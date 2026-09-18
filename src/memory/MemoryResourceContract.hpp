#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "MemoryComposition.hpp"
#include "MemoryTypes.hpp"

namespace ESPressio::Memory::Detail {

    /// Validates the complete public contract required from a MemoryResource provider.
    template<class TMemoryResourceProvider>
    struct MemoryResourceProviderTraits {

        static_assert(
            TMemoryResourceProvider::CompositionCapabilities::template Contains<MemoryResource>,
            "MemoryResource provider must supply the MemoryResource capability"
        );

        // Operation return types.

        /// Return type produced by Allocate.
        using AllocateResult = decltype(
            std::declval<TMemoryResourceProvider&>().Allocate(
                std::declval<std::size_t>(),
                std::declval<std::size_t>(),
                std::declval<MemoryBlock&>()
            )
        );

        /// Return type produced by Release.
        using ReleaseResult = decltype(
            std::declval<TMemoryResourceProvider&>().Release(
                std::declval<const MemoryBlock&>()
            )
        );

        static_assert(
            std::is_same_v<AllocateResult, MemoryAllocationResult>,
            "MemoryResource Allocate must return MemoryAllocationResult"
        );

        static_assert(
            std::is_same_v<ReleaseResult, MemoryReleaseResult>,
            "MemoryResource Release must return MemoryReleaseResult"
        );

        static_assert(
            noexcept(
                std::declval<TMemoryResourceProvider&>().Allocate(
                    std::declval<std::size_t>(),
                    std::declval<std::size_t>(),
                    std::declval<MemoryBlock&>()
                )
            ),
            "MemoryResource Allocate must be noexcept"
        );

        static_assert(
            noexcept(
                std::declval<TMemoryResourceProvider&>().Release(
                    std::declval<const MemoryBlock&>()
                )
            ),
            "MemoryResource Release must be noexcept"
        );

    };

} // ESPressio::Memory::Detail
