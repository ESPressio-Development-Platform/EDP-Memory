#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ESPressio::Memory {

    /// Declares the exact dedicated instance count reserved for one pooled Type.
    template<std::size_t TCount>
    struct DedicatedInstances final {

        static_assert(
            TCount <= 255U,
            "DedicatedInstances must fit the locked uint8_t capacity range"
        );

        // Compile-time capacity.

        /// Exact dedicated instance count.
        static constexpr std::uint8_t Value = static_cast<std::uint8_t>(TCount);

    };


    /// Declares that one pooled Type may never consume the shared reserve.
    struct NoSharedOverflow final {

        // Compile-time shared-overflow policy.

        /// Indicates that shared-reserve allocation is disabled for this Type.
        static constexpr bool IsEnabled = false;

        /// Maximum simultaneous shared instances; unused when shared overflow is disabled.
        static constexpr std::uint8_t MaximumInstances = 0U;

    };


    /// Declares shared-reserve eligibility and an optional per-Type simultaneous-instance quota.
    template<std::size_t TMaximumInstances>
    struct SharedOverflow final {

        static_assert(
            TMaximumInstances <= 255U,
            "SharedOverflow quota must fit the locked uint8_t capacity range"
        );

        // Compile-time shared-overflow policy.

        /// Indicates that shared-reserve allocation is enabled for this Type.
        static constexpr bool IsEnabled = true;

        /// Maximum simultaneous shared instances, or zero for no per-Type quota.
        static constexpr std::uint8_t MaximumInstances = static_cast<std::uint8_t>(TMaximumInstances);

    };


    /// Selects the topology-wide default MemoryResource for one Object Pool.
    struct UseDefaultMemoryResource final {};


    /// Selects one explicit MemoryResource provider for one Object Pool.
    ///
    /// @tparam TMemoryResourceProvider Concrete MemoryResource provider selected by Bootstrap.
    template<class TMemoryResourceProvider>
    struct UseMemoryResource final {

        // Selected provider.

        /// Concrete MemoryResource provider Type used by this Object Pool.
        using Provider = TMemoryResourceProvider;

    };


    /// Describes one pooled Type and its deterministic dedicated/shared capacity policy.
    ///
    /// @tparam TObject Object Type whose instances are managed by this pool.
    /// @tparam TDedicatedInstances DedicatedInstances declaration for TObject.
    /// @tparam TSharedOverflow Shared overflow policy for TObject.
    /// @tparam TMemoryResourceSelection Default or explicit dedicated MemoryResource selection.
    template<
        class TObject,
        class TDedicatedInstances,
        class TSharedOverflow = NoSharedOverflow,
        class TMemoryResourceSelection = UseDefaultMemoryResource
    >
    struct ObjectPoolSpec final {

        static_assert(
            std::is_nothrow_destructible_v<TObject>,
            "ObjectPool managed Types must be nothrow destructible"
        );

        // Object Pool configuration.

        /// Object Type managed by this pool.
        using Object = TObject;

        /// Dedicated capacity declaration.
        using Dedicated = TDedicatedInstances;

        /// Shared overflow policy.
        using Shared = TSharedOverflow;

        /// Dedicated MemoryResource selection.
        using ResourceSelection = TMemoryResourceSelection;

    };


    /// Describes the one bounded shared reserve used by a Memory topology.
    ///
    /// @tparam TBytes Absolute configured reserve bytes, including allocator metadata and padding.
    /// @tparam TMemoryResourceProvider Concrete MemoryResource provider backing the reserve.
    template<
        std::size_t TBytes,
        class TMemoryResourceProvider
    >
    struct SharedReserve final {

        // Shared reserve configuration.

        /// Absolute configured shared-reserve byte count.
        static constexpr std::size_t Bytes = TBytes;

        /// Concrete MemoryResource provider backing the shared reserve.
        using ResourceProvider = TMemoryResourceProvider;

    };

} // ESPressio::Memory
