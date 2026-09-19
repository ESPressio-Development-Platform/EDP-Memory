#pragma once

#include <cstddef>
#include <cstdint>
#include <new>

#include "MemoryComposition.hpp"
#include "MemoryTypes.hpp"

namespace ESPressio::Memory {

    /// Deterministic first-fit allocator for one fixed shared reserve.
    ///
    /// Metadata is stored in-band inside the reserve. Allocation always searches from the beginning,
    /// live payloads are never moved, and release immediately coalesces adjacent free spans.
    class CoalescingFirstFitProvider final : public Framework::Provider<
        Domain,
        Framework::Provides<
            Framework::Offer<SharedReserveAllocationAlgorithm>
        >
    > {

        private:

            // In-band block representation.

            /// In-band header describing one contiguous span of the shared reserve.
            struct BlockHeader final {

                // Span description.

                /// Total bytes occupied by this span, including this header.
                std::size_t SpanBytes = 0U;

                /// Payload offset from this header, or zero when the span is free.
                std::size_t PayloadOffset = 0U;

            };


            // Shared reserve state.

            /// First byte of the currently initialized reserve.
            std::byte* _base = nullptr;

            /// Header-aligned usable byte count in the currently initialized reserve.
            std::size_t _size = 0U;


            // Alignment helpers.

            /// Reports whether a non-zero integer is a power of two.
            static constexpr bool IsPowerOfTwo(
                std::size_t value
            ) noexcept {
                return value != 0U && (value & (value - 1U)) == 0U;
            }

            /// Rounds a value upward to the requested power-of-two alignment.
            static constexpr std::size_t AlignUp(
                std::size_t value,
                std::size_t alignment
            ) noexcept {
                return (value + (alignment - 1U)) & ~(alignment - 1U);
            }

            /// Returns the first block header in the reserve.
            BlockHeader* FirstHeader() noexcept {
                return reinterpret_cast<BlockHeader*>(_base);
            }

            /// Returns the first block header in the reserve for const inspection.
            const BlockHeader* FirstHeader() const noexcept {
                return reinterpret_cast<const BlockHeader*>(_base);
            }

            /// Returns the byte offset of a header from the reserve start.
            std::size_t HeaderOffset(
                const BlockHeader& header
            ) const noexcept {
                return static_cast<std::size_t>(
                    reinterpret_cast<const std::byte*>(&header) - _base
                );
            }

            /// Returns the next header, or null when the supplied block reaches the reserve end.
            BlockHeader* NextHeader(
                BlockHeader& header
            ) noexcept {
                const auto offset = HeaderOffset(header);
                const auto nextOffset = offset + header.SpanBytes;

                if (nextOffset >= _size) { return nullptr; }

                return reinterpret_cast<BlockHeader*>(_base + nextOffset);
            }

            /// Returns the next header for const inspection.
            const BlockHeader* NextHeader(
                const BlockHeader& header
            ) const noexcept {
                const auto offset = HeaderOffset(header);
                const auto nextOffset = offset + header.SpanBytes;

                if (nextOffset >= _size) { return nullptr; }

                return reinterpret_cast<const BlockHeader*>(_base + nextOffset);
            }

        public:

            // Construction and lifetime.

            /// Creates an uninitialized shared-reserve allocator.
            CoalescingFirstFitProvider() = default;

            /// Prevents copying allocator runtime state.
            CoalescingFirstFitProvider(const CoalescingFirstFitProvider&) = delete;

            /// Prevents copy assignment of allocator runtime state.
            CoalescingFirstFitProvider& operator =(const CoalescingFirstFitProvider&) = delete;

            /// Prevents moving allocator runtime state.
            CoalescingFirstFitProvider(CoalescingFirstFitProvider&&) = delete;

            /// Prevents move assignment of allocator runtime state.
            CoalescingFirstFitProvider& operator =(CoalescingFirstFitProvider&&) = delete;


            // Shared reserve lifecycle.

            /// Initializes this allocator over one fixed backing block.
            SharedReserveInitializationResult Initialize(
                const MemoryBlock& block
            ) noexcept {
                if (_base != nullptr) { return SharedReserveInitializationResult::AlreadyInitialized; }

                if (
                    block.Address == nullptr ||
                    block.Size < (sizeof(BlockHeader) + alignof(BlockHeader)) ||
                    block.Alignment < alignof(BlockHeader) ||
                    (reinterpret_cast<std::uintptr_t>(block.Address) % alignof(BlockHeader)) != 0U
                ) {
                    return SharedReserveInitializationResult::InvalidBlock;
                }

                const auto usableSize = block.Size - (block.Size % alignof(BlockHeader));

                if (usableSize < (sizeof(BlockHeader) + alignof(BlockHeader))) {
                    return SharedReserveInitializationResult::InvalidBlock;
                }

                _base = static_cast<std::byte*>(block.Address);
                _size = usableSize;

                ::new (static_cast<void*>(_base)) BlockHeader {
                    _size,
                    0U
                };

                return SharedReserveInitializationResult::Succeeded;
            }

            /// Tears down this allocator only when every shared allocation has been returned.
            SharedAllocatorTeardownResult TearDown() noexcept {
                if (_base == nullptr) { return SharedAllocatorTeardownResult::NotInitialized; }
                if (HasLiveAllocations()) { return SharedAllocatorTeardownResult::AllocationsRemain; }

                _base = nullptr;
                _size = 0U;

                return SharedAllocatorTeardownResult::Succeeded;
            }


            // Shared allocation operations.

            /// Allocates the first suitably aligned contiguous span from the shared reserve.
            SharedAllocationResult Allocate(
                std::size_t byteCount,
                std::size_t alignment,
                SharedAllocation& allocation
            ) noexcept {
                if (_base == nullptr) { return SharedAllocationResult::NotInitialized; }
                if (byteCount == 0U) { return SharedAllocationResult::InvalidSize; }
                if (!IsPowerOfTwo(alignment)) { return SharedAllocationResult::InvalidAlignment; }

                std::size_t aggregateFreePayload = 0U;
                auto* header = FirstHeader();

                while (header != nullptr) {
                    if (header->PayloadOffset == 0U) {
                        if (header->SpanBytes > sizeof(BlockHeader)) {
                            aggregateFreePayload += header->SpanBytes - sizeof(BlockHeader);
                        }

                        const auto headerOffset = HeaderOffset(*header);
                        const auto payloadCandidateAddress = reinterpret_cast<std::uintptr_t>(
                            _base + headerOffset + sizeof(BlockHeader)
                        );

                        if (
                            payloadCandidateAddress >
                            (static_cast<std::uintptr_t>(-1) - static_cast<std::uintptr_t>(alignment - 1U))
                        ) {
                            header = NextHeader(*header);
                            continue;
                        }

                        const auto alignedPayloadAddress = static_cast<std::uintptr_t>(
                            AlignUp(
                                static_cast<std::size_t>(payloadCandidateAddress),
                                alignment
                            )
                        );
                        const auto payloadOffsetFromReserve = static_cast<std::size_t>(
                            alignedPayloadAddress - reinterpret_cast<std::uintptr_t>(_base)
                        );
                        const auto payloadOffsetFromHeader = payloadOffsetFromReserve - headerOffset;

                        if (payloadOffsetFromHeader <= header->SpanBytes) {
                            const auto requiredBytes = payloadOffsetFromHeader + byteCount;

                            if (requiredBytes <= header->SpanBytes) {
                                const auto alignedRequiredBytes = AlignUp(
                                    requiredBytes,
                                    alignof(BlockHeader)
                                );
                                auto allocationSpan = alignedRequiredBytes;

                                if (allocationSpan > header->SpanBytes) {
                                    allocationSpan = header->SpanBytes;
                                }

                                const auto remainder = header->SpanBytes - allocationSpan;
                                const auto minimumRepresentableRemainder = sizeof(BlockHeader) + alignof(BlockHeader);

                                if (remainder >= minimumRepresentableRemainder) {
                                    auto* next = reinterpret_cast<BlockHeader*>(
                                        reinterpret_cast<std::byte*>(header) + allocationSpan
                                    );

                                    ::new (static_cast<void*>(next)) BlockHeader {
                                        remainder,
                                        0U
                                    };

                                    header->SpanBytes = allocationSpan;
                                }

                                header->PayloadOffset = payloadOffsetFromHeader;

                                SharedAllocation candidate;
                                candidate.PayloadOffset = payloadOffsetFromReserve;
                                allocation = candidate;

                                return SharedAllocationResult::Succeeded;
                            }
                        }
                    }

                    header = NextHeader(*header);
                }

                return aggregateFreePayload >= byteCount
                    ? SharedAllocationResult::ContiguousCapacityUnavailable
                    : SharedAllocationResult::CapacityUnavailable;
            }

            /// Releases one previously allocated shared span and immediately coalesces free neighbors.
            SharedAllocationReleaseResult Release(
                const SharedAllocation& allocation
            ) noexcept {
                if (_base == nullptr) { return SharedAllocationReleaseResult::NotInitialized; }
                if (allocation.PayloadOffset == 0U || allocation.PayloadOffset >= _size) {
                    return SharedAllocationReleaseResult::InvalidAllocation;
                }

                BlockHeader* previous = nullptr;
                auto* header = FirstHeader();

                while (header != nullptr) {
                    const auto headerOffset = HeaderOffset(*header);

                    if (
                        header->PayloadOffset != 0U &&
                        headerOffset + header->PayloadOffset == allocation.PayloadOffset
                    ) {
                        header->PayloadOffset = 0U;

                        auto* next = NextHeader(*header);

                        if (next != nullptr && next->PayloadOffset == 0U) {
                            header->SpanBytes += next->SpanBytes;
                        }

                        if (previous != nullptr && previous->PayloadOffset == 0U) {
                            previous->SpanBytes += header->SpanBytes;
                        }

                        return SharedAllocationReleaseResult::Released;
                    }

                    previous = header;
                    header = NextHeader(*header);
                }

                return SharedAllocationReleaseResult::InvalidAllocation;
            }

            /// Resolves the live payload address associated with one shared allocation token.
            void* AddressOf(
                const SharedAllocation& allocation
            ) noexcept {
                if (
                    _base == nullptr ||
                    allocation.PayloadOffset == 0U ||
                    allocation.PayloadOffset >= _size
                ) {
                    return nullptr;
                }

                return static_cast<void*>(_base + allocation.PayloadOffset);
            }


            // Shared reserve inspection.

            /// Reports whether any allocated span remains live in the shared reserve.
            bool HasLiveAllocations() const noexcept {
                if (_base == nullptr) { return false; }

                const auto* header = FirstHeader();

                while (header != nullptr) {
                    if (header->PayloadOffset != 0U) { return true; }

                    header = NextHeader(*header);
                }

                return false;
            }

    };

} // ESPressio::Memory
