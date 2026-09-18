#pragma once

#include <cstddef>
#include <tuple>

#include "ObjectPoolConfiguration.hpp"
#include "detail/TopologyTraits.hpp"

namespace ESPressio::Memory {

    /// Complete compile-time declaration of one immutable Memory topology.
    ///
    /// @tparam TDefaultMemoryResourceProvider Default dedicated MemoryResource provider selected by Bootstrap.
    /// @tparam TSharedReserve SharedReserve declaration for the one v1 overflow reserve.
    /// @tparam TObjectPoolSpecs ObjectPoolSpec declarations comprising the topology.
    template<
        class TDefaultMemoryResourceProvider,
        class TSharedReserve,
        class... TObjectPoolSpecs
    >
    struct MemoryTopology final {

        static_assert(
            Detail::UniqueObjectPoolTypes<TObjectPoolSpecs...>::value,
            "MemoryTopology may contain no more than one ObjectPoolSpec for each object Type"
        );

        static_assert(
            !Detail::AnySharedOverflowEnabledV<TObjectPoolSpecs...> || TSharedReserve::Bytes > 0U,
            "A topology with shared-eligible Object Pools requires a non-zero shared reserve"
        );

        // Topology configuration.

        /// Default dedicated MemoryResource provider.
        using DefaultMemoryResourceProvider = TDefaultMemoryResourceProvider;

        /// One shared-reserve declaration for this topology.
        using SharedReserveSpec = TSharedReserve;

        /// Ordered tuple of ObjectPoolSpec declarations.
        using ObjectPoolSpecs = std::tuple<TObjectPoolSpecs...>;

        /// Number of Object Pools configured by this topology.
        static constexpr std::size_t ObjectPoolCount = sizeof...(TObjectPoolSpecs);

        /// Reports whether this topology contains an Object Pool for TObject.
        template<class TObject>
        static constexpr bool ContainsObjectPool = !std::is_void_v<
            typename Detail::FindObjectPoolSpec<TObject, TObjectPoolSpecs...>::Type
        >;

        /// Resolves the ObjectPoolSpec configured for TObject.
        template<class TObject>
        using ObjectPoolSpecFor = typename Detail::FindObjectPoolSpec<TObject, TObjectPoolSpecs...>::Type;

        /// Returns the zero-based declaration ordinal of TObject's ObjectPoolSpec.
        template<class TObject>
        static constexpr std::size_t ObjectPoolIndex = Detail::FindObjectPoolSpec<
            TObject,
            TObjectPoolSpecs...
        >::Index;

        /// Resolves the dedicated MemoryResource provider selected for one ObjectPoolSpec.
        template<class TObjectPoolSpec>
        using ResourceProviderFor = typename Detail::ResolveObjectPoolResource<
            TDefaultMemoryResourceProvider,
            typename TObjectPoolSpec::ResourceSelection
        >::Type;

    };

} // ESPressio::Memory
