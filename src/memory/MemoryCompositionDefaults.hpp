#pragma once

#include <cstddef>

#include "CoalescingFirstFitProvider.hpp"
#include "MemoryComposition.hpp"

namespace ESPressio::Memory::Detail {

    /// Counts providers that explicitly supply the shared-reserve allocation capability.
    template<class... TProviders>
    inline constexpr std::size_t SharedAllocatorProviderCountV = (
        std::size_t{0U} + ... + (
            TProviders::CompositionOffers::template Contains<SharedReserveAllocationAlgorithm>
                ? std::size_t{1U}
                : std::size_t{0U}
        )
    );


    /// Selects the explicit shared-reserve algorithm or appends CoalescingFirstFitProvider by default.
    template<
        bool THasExplicitProvider,
        class... TProviders
    >
    struct DefaultMemoryCompositionSelector;


    /// Builds a Memory Composition retaining the explicitly selected allocator provider.
    template<class... TProviders>
    struct DefaultMemoryCompositionSelector<true, TProviders...> {

        // Composition result.

        /// Complete Memory Composition using the explicitly supplied allocation algorithm.
        using Type = Framework::Composition<
            Domain,
            TProviders...
        >;

    };


    /// Builds a Memory Composition using the default coalescing-first-fit allocation algorithm.
    template<class... TProviders>
    struct DefaultMemoryCompositionSelector<false, TProviders...> {

        // Composition result.

        /// Complete Memory Composition with CoalescingFirstFitProvider appended at compile time.
        using Type = Framework::Composition<
            Domain,
            TProviders...,
            CoalescingFirstFitProvider
        >;

    };

} // ESPressio::Memory::Detail

namespace ESPressio::Memory {

    /// Builds one compile-time Memory Composition and injects CoalescingFirstFitProvider when no
    /// shared-reserve allocation algorithm is explicitly supplied by Bootstrap.
    ///
    /// @tparam TProviders Explicit Memory capability providers selected by application Bootstrap.
    template<class... TProviders>
    using MemoryComposition = typename Detail::DefaultMemoryCompositionSelector<
        Detail::SharedAllocatorProviderCountV<TProviders...> != 0U,
        TProviders...
    >::Type;

    static_assert(
        Detail::SharedAllocatorProviderCountV<CoalescingFirstFitProvider> == 1U,
        "Default shared-reserve provider must advertise exactly one allocation capability"
    );

} // ESPressio::Memory
