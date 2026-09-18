#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "../ObjectPoolConfiguration.hpp"

namespace ESPressio::Memory::Detail {

    /// Indicates whether one Type appears in a Type pack.
    template<class TNeedle, class... THaystack>
    inline constexpr bool ContainsTypeV = (std::is_same_v<TNeedle, THaystack> || ...);


    /// Validates that every ObjectPoolSpec owns a distinct object Type.
    template<class... TObjectPoolSpecs>
    struct UniqueObjectPoolTypes;


    /// Empty pool-specification packs are unique.
    template<>
    struct UniqueObjectPoolTypes<> : std::true_type {};


    /// Checks one pool Type against the remaining pool Types recursively.
    template<class TFirstSpec, class... TRestSpecs>
    struct UniqueObjectPoolTypes<TFirstSpec, TRestSpecs...> : std::bool_constant<
        (!std::is_same_v<typename TFirstSpec::Object, typename TRestSpecs::Object> && ...) &&
        UniqueObjectPoolTypes<TRestSpecs...>::value
    > {};


    /// Resolves an ObjectPoolSpec by its managed object Type.
    template<class TObject, class... TObjectPoolSpecs>
    struct FindObjectPoolSpec;


    /// Represents an unsuccessful ObjectPoolSpec lookup.
    template<class TObject>
    struct FindObjectPoolSpec<TObject> {

        // Lookup result.

        /// Missing specification marker.
        using Type = void;

        /// Sentinel index returned when no matching ObjectPoolSpec exists.
        static constexpr std::size_t Index = static_cast<std::size_t>(-1);

    };


    /// Resolves the first ObjectPoolSpec managing TObject.
    template<class TObject, class TFirstSpec, class... TRestSpecs>
    struct FindObjectPoolSpec<TObject, TFirstSpec, TRestSpecs...> {

        private:

            // Remaining lookup.

            /// Recursive lookup result for the remaining specifications.
            using Remaining = FindObjectPoolSpec<TObject, TRestSpecs...>;

        public:

            // Lookup result.

            /// Matching ObjectPoolSpec, or the recursive result when the first specification differs.
            using Type = std::conditional_t<
                std::is_same_v<TObject, typename TFirstSpec::Object>,
                TFirstSpec,
                typename Remaining::Type
            >;

            /// Zero-based topology ordinal of the matching ObjectPoolSpec.
            static constexpr std::size_t Index = std::is_same_v<TObject, typename TFirstSpec::Object>
                ? 0U
                : (
                    Remaining::Index == static_cast<std::size_t>(-1)
                        ? static_cast<std::size_t>(-1)
                        : Remaining::Index + 1U
                );

    };


    /// Resolves the dedicated MemoryResource provider selected by one ObjectPoolSpec.
    template<class TDefaultMemoryResourceProvider, class TResourceSelection>
    struct ResolveObjectPoolResource;


    /// Resolves the topology default MemoryResource provider.
    template<class TDefaultMemoryResourceProvider>
    struct ResolveObjectPoolResource<TDefaultMemoryResourceProvider, UseDefaultMemoryResource> {

        // Resolution result.

        /// Concrete MemoryResource provider selected for the pool.
        using Type = TDefaultMemoryResourceProvider;

    };


    /// Resolves one explicit Object Pool MemoryResource override.
    template<class TDefaultMemoryResourceProvider, class TMemoryResourceProvider>
    struct ResolveObjectPoolResource<
        TDefaultMemoryResourceProvider,
        UseMemoryResource<TMemoryResourceProvider>
    > {

        // Resolution result.

        /// Concrete MemoryResource provider selected for the pool.
        using Type = TMemoryResourceProvider;

    };


    /// Indicates whether any ObjectPoolSpec enables shared overflow.
    template<class... TObjectPoolSpecs>
    inline constexpr bool AnySharedOverflowEnabledV = (false || ... || TObjectPoolSpecs::Shared::IsEnabled);


    /// Indicates whether a provider list contains a specific provider Type.
    template<class TProvider, class TProviderList>
    struct ProviderListContains;


    /// Checks one Composition ProviderList for a specific provider Type.
    template<class TProvider, class... TProviders>
    struct ProviderListContains<TProvider, ESPressio::System::CompositionFramework::ProviderList<TProviders...>> : std::bool_constant<
        ContainsTypeV<TProvider, TProviders...>
    > {};

} // ESPressio::Memory::Detail
