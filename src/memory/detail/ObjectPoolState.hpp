#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "../MemoryResourceContract.hpp"
#include "../ObjectPoolConfiguration.hpp"

namespace ESPressio::Memory::Detail {

    /// Empty dedicated state used when an Object Pool reserves no dedicated instances.
    template<class TObject, std::size_t TCount>
    class DedicatedObjectPoolState;


    /// Zero-capacity dedicated state requiring no backing descriptor or occupancy bytes.
    template<class TObject>
    class DedicatedObjectPoolState<TObject, 0U> {

        public:

            /// Reports that this pool has no dedicated live objects.
            bool HasLiveDedicatedObjects() const noexcept {
                return false;
            }

            /// Reports that no dedicated slot can be claimed.
            bool TryClaimDedicated(
                std::size_t&
            ) noexcept {
                return false;
            }

            /// Ignores release because no dedicated slot exists.
            void ReleaseDedicated(
                std::size_t
            ) noexcept {
            }

            /// Returns null because no dedicated slot exists.
            TObject* DedicatedAddress(
                std::size_t
            ) noexcept {
                return nullptr;
            }

            /// Performs no allocation for a zero-capacity dedicated pool.
            template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider&
            ) noexcept {
                return MemoryAllocationResult::Succeeded;
            }

            /// Performs no release for a zero-capacity dedicated pool.
            template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider&
            ) noexcept {
                return MemoryReleaseResult::Released;
            }

    };


    /// Fixed dedicated storage and occupancy metadata for a non-zero Object Pool capacity.
    template<class TObject, std::size_t TCount>
    class DedicatedObjectPoolState {

        private:

            static_assert(
                TCount > 0U,
                "Non-zero DedicatedObjectPoolState specialization requires positive capacity"
            );

            // Dedicated backing state.

            /// Raw backing allocation containing exactly TCount object slots.
            MemoryBlock _backing{};

            /// One occupancy bit per dedicated object slot.
            std::array<std::uint8_t, (TCount + 7U) / 8U> _occupancy{};

            /// Reports whether one dedicated slot is occupied.
            bool IsOccupied(
                std::size_t slotIndex
            ) const noexcept {
                const auto byteIndex = slotIndex / 8U;
                const auto bitIndex = slotIndex % 8U;
                const auto mask = static_cast<std::uint8_t>(1U << bitIndex);

                return (_occupancy[byteIndex] & mask) != 0U;
            }

            /// Sets or clears one dedicated slot occupancy bit.
            void SetOccupied(
                std::size_t slotIndex,
                bool occupied
            ) noexcept {
                const auto byteIndex = slotIndex / 8U;
                const auto bitIndex = slotIndex % 8U;
                const auto mask = static_cast<std::uint8_t>(1U << bitIndex);

                if (occupied) {
                    _occupancy[byteIndex] = static_cast<std::uint8_t>(_occupancy[byteIndex] | mask);
                } else {
                    _occupancy[byteIndex] = static_cast<std::uint8_t>(_occupancy[byteIndex] & static_cast<std::uint8_t>(~mask));
                }
            }

        public:

            /// Acquires exactly one contiguous backing allocation for every dedicated slot.
            template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider& resource
            ) noexcept {
                static_assert(
                    TCount <= (static_cast<std::size_t>(-1) / sizeof(TObject)),
                    "Dedicated Object Pool byte requirement overflows size_t"
                );

                MemoryBlock candidate{};
                const auto result = resource.Allocate(
                    sizeof(TObject) * TCount,
                    alignof(TObject),
                    candidate
                );

                if (result != MemoryAllocationResult::Succeeded) { return result; }

                _backing = candidate;
                _occupancy.fill(0U);

                return MemoryAllocationResult::Succeeded;
            }

            /// Releases the complete dedicated backing allocation when no live object remains.
            template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider& resource
            ) noexcept {
                if (_backing.Address == nullptr) { return MemoryReleaseResult::Released; }

                const auto result = resource.Release(_backing);

                if (result == MemoryReleaseResult::Released) {
                    _backing = MemoryBlock{};
                    _occupancy.fill(0U);
                }

                return result;
            }

            /// Claims the lowest-index vacant dedicated slot.
            bool TryClaimDedicated(
                std::size_t& slotIndex
            ) noexcept {
                for (std::size_t index = 0U; index < TCount; ++index) {
                    if (!IsOccupied(index)) {
                        SetOccupied(
                            index,
                            true
                        );
                        slotIndex = index;
                        return true;
                    }
                }

                return false;
            }

            /// Returns one previously claimed dedicated slot to the vacant set.
            void ReleaseDedicated(
                std::size_t slotIndex
            ) noexcept {
                if (slotIndex < TCount) {
                    SetOccupied(
                        slotIndex,
                        false
                    );
                }
            }

            /// Resolves the object address for one dedicated slot.
            TObject* DedicatedAddress(
                std::size_t slotIndex
            ) noexcept {
                if (_backing.Address == nullptr || slotIndex >= TCount) { return nullptr; }

                auto* bytes = static_cast<std::byte*>(_backing.Address);

                return reinterpret_cast<TObject*>(
                    bytes + (slotIndex * sizeof(TObject))
                );
            }

            /// Reports whether any dedicated slot is currently claimed.
            bool HasLiveDedicatedObjects() const noexcept {
                for (const auto byte : _occupancy)
                    if (byte != 0U) { return true; }

                return false;
            }

    };


    /// Shared-overflow quota state selected from one ObjectPoolSpec policy.
    template<
        class TSharedPolicy,
        bool TTracksQuota = TSharedPolicy::IsEnabled && (TSharedPolicy::MaximumInstances > 0U)
    >
    class SharedObjectPoolQuotaState;


    /// Empty state used when shared overflow is disabled or carries no per-Type quota.
    template<class TSharedPolicy>
    class SharedObjectPoolQuotaState<TSharedPolicy, false> {

        public:

            /// Reports whether this policy permits attempting a shared allocation.
            bool CanClaimShared() const noexcept {
                return TSharedPolicy::IsEnabled;
            }

            /// Records no quota state because this policy has no finite per-Type quota.
            void ClaimShared() noexcept {
            }

            /// Releases no quota state because this policy has no finite per-Type quota.
            void ReleaseShared() noexcept {
            }

    };


    /// One-byte quota state used only when a finite shared instance limit is configured.
    template<class TSharedPolicy>
    class SharedObjectPoolQuotaState<TSharedPolicy, true> {

        private:

            // Shared quota state.

            /// Current simultaneous shared-reserve instance count for this Object Pool.
            std::uint8_t _count = 0U;

        public:

            /// Reports whether another shared instance is permitted by the configured quota.
            bool CanClaimShared() const noexcept {
                return _count < TSharedPolicy::MaximumInstances;
            }

            /// Records one newly claimed shared instance.
            void ClaimShared() noexcept {
                ++_count;
            }

            /// Records return of one shared instance.
            void ReleaseShared() noexcept {
                if (_count > 0U) { --_count; }
            }

    };

} // ESPressio::Memory::Detail
