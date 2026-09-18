#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "MemoryComposition.hpp"
#include "MemoryTypes.hpp"

namespace ESPressio::Memory::Detail {

    /// Validates the complete public contract required from a ByteOperations provider.
    template<class TByteOperationsProvider>
    struct ByteOperationsProviderTraits {

        static_assert(
            TByteOperationsProvider::CompositionCapabilities::template Contains<ByteOperations>,
            "ByteOperations provider must supply the ByteOperations capability"
        );

        // Operation return types.

        /// Return type produced by CopyBytes.
        using CopyResult = decltype(
            std::declval<const TByteOperationsProvider&>().CopyBytes(
                std::declval<void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );

        /// Return type produced by MoveBytes.
        using MoveResult = decltype(
            std::declval<const TByteOperationsProvider&>().MoveBytes(
                std::declval<void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );

        /// Return type produced by FillBytes.
        using FillResult = decltype(
            std::declval<const TByteOperationsProvider&>().FillBytes(
                std::declval<void*>(),
                std::declval<std::uint8_t>(),
                std::declval<std::size_t>()
            )
        );

        /// Return type produced by CompareBytes.
        using CompareResult = decltype(
            std::declval<const TByteOperationsProvider&>().CompareBytes(
                std::declval<const void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );

        static_assert(
            std::is_same_v<CopyResult, void> &&
            std::is_same_v<MoveResult, void> &&
            std::is_same_v<FillResult, void>,
            "ByteOperations mutation methods must return void"
        );

        static_assert(
            std::is_same_v<CompareResult, ByteComparison>,
            "ByteOperations CompareBytes must return ByteComparison"
        );

        static_assert(
            noexcept(
                std::declval<const TByteOperationsProvider&>().CopyBytes(
                    std::declval<void*>(),
                    std::declval<const void*>(),
                    std::declval<std::size_t>()
                )
            ),
            "ByteOperations CopyBytes must be noexcept"
        );

        static_assert(
            noexcept(
                std::declval<const TByteOperationsProvider&>().MoveBytes(
                    std::declval<void*>(),
                    std::declval<const void*>(),
                    std::declval<std::size_t>()
                )
            ),
            "ByteOperations MoveBytes must be noexcept"
        );

        static_assert(
            noexcept(
                std::declval<const TByteOperationsProvider&>().FillBytes(
                    std::declval<void*>(),
                    std::declval<std::uint8_t>(),
                    std::declval<std::size_t>()
                )
            ),
            "ByteOperations FillBytes must be noexcept"
        );

        static_assert(
            noexcept(
                std::declval<const TByteOperationsProvider&>().CompareBytes(
                    std::declval<const void*>(),
                    std::declval<const void*>(),
                    std::declval<std::size_t>()
                )
            ),
            "ByteOperations CompareBytes must be noexcept"
        );

    };

} // ESPressio::Memory::Detail
