#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "MemoryComposition.hpp"
#include "MemoryTypes.hpp"

namespace ESPressio::Memory::Detail {

    /// Validates the complete public contract required from a SharedReserveAllocationAlgorithm provider.
    template<class TSharedReserveAllocationProvider>
    struct SharedReserveAllocationProviderTraits {

        static_assert(
            TSharedReserveAllocationProvider::CompositionCapabilities::template Contains<SharedReserveAllocationAlgorithm>,
            "Shared-reserve allocator provider must supply SharedReserveAllocationAlgorithm"
        );

        // Operation return types.

        /// Return type produced by Initialize.
        using InitializeResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Initialize(
                std::declval<const MemoryBlock&>()
            )
        );

        /// Return type produced by Allocate.
        using AllocateResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Allocate(
                std::declval<std::size_t>(),
                std::declval<std::size_t>(),
                std::declval<SharedAllocation&>()
            )
        );

        /// Return type produced by Release.
        using ReleaseResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Release(
                std::declval<const SharedAllocation&>()
            )
        );

        /// Return type produced by TearDown.
        using TearDownResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().TearDown()
        );

        /// Return type produced by the live-allocation predicate.
        using HasLiveAllocationsResult = decltype(
            std::declval<const TSharedReserveAllocationProvider&>().HasLiveAllocations()
        );

        /// Return type produced by resolving a shared allocation address.
        using AddressResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().AddressOf(
                std::declval<const SharedAllocation&>()
            )
        );

        static_assert(
            std::is_same_v<InitializeResult, SharedReserveInitializationResult>,
            "Shared-reserve Initialize must return SharedReserveInitializationResult"
        );

        static_assert(
            std::is_same_v<AllocateResult, SharedAllocationResult>,
            "Shared-reserve Allocate must return SharedAllocationResult"
        );

        static_assert(
            std::is_same_v<ReleaseResult, SharedAllocationReleaseResult>,
            "Shared-reserve Release must return SharedAllocationReleaseResult"
        );

        static_assert(
            std::is_same_v<TearDownResult, SharedAllocatorTeardownResult>,
            "Shared-reserve TearDown must return SharedAllocatorTeardownResult"
        );

        static_assert(
            std::is_same_v<HasLiveAllocationsResult, bool>,
            "Shared-reserve HasLiveAllocations must return bool"
        );

        static_assert(
            std::is_same_v<AddressResult, void*>,
            "Shared-reserve AddressOf must return void*"
        );

        static_assert(
            noexcept(
                std::declval<TSharedReserveAllocationProvider&>().Initialize(
                    std::declval<const MemoryBlock&>()
                )
            ) &&
            noexcept(
                std::declval<TSharedReserveAllocationProvider&>().Allocate(
                    std::declval<std::size_t>(),
                    std::declval<std::size_t>(),
                    std::declval<SharedAllocation&>()
                )
            ) &&
            noexcept(
                std::declval<TSharedReserveAllocationProvider&>().Release(
                    std::declval<const SharedAllocation&>()
                )
            ) &&
            noexcept(
                std::declval<TSharedReserveAllocationProvider&>().TearDown()
            ) &&
            noexcept(
                std::declval<const TSharedReserveAllocationProvider&>().HasLiveAllocations()
            ) &&
            noexcept(
                std::declval<TSharedReserveAllocationProvider&>().AddressOf(
                    std::declval<const SharedAllocation&>()
                )
            ),
            "Shared-reserve provider operations must be noexcept"
        );

    };

} // ESPressio::Memory::Detail
