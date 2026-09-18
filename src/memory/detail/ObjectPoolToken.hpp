#pragma once

#include <cstddef>
#include <limits>

#include "../MemoryTypes.hpp"

namespace ESPressio::Memory::Detail {

    /// Encodes dedicated/shared object identity and release phase into one machine word.
    struct ObjectPoolToken final {

        // Bit layout.

        /// High bit identifying a shared-reserve allocation.
        static constexpr std::size_t SharedFlag =
            std::size_t{1U} << (std::numeric_limits<std::size_t>::digits - 1U);

        /// Next-highest bit identifying an object whose destructor has completed but whose storage is not yet returned.
        static constexpr std::size_t DestroyedFlag =
            std::size_t{1U} << (std::numeric_limits<std::size_t>::digits - 2U);

        /// Mask containing the dedicated slot ordinal or shared payload offset.
        static constexpr std::size_t ValueMask = ~(SharedFlag | DestroyedFlag);

        // Token construction.

        /// Creates an empty token.
        static constexpr std::size_t Empty() noexcept {
            return 0U;
        }

        /// Encodes one zero-based dedicated slot index.
        static constexpr std::size_t Dedicated(
            std::size_t slotIndex
        ) noexcept {
            return slotIndex + 1U;
        }

        /// Encodes one shared-reserve payload offset.
        static constexpr std::size_t Shared(
            std::size_t payloadOffset
        ) noexcept {
            return SharedFlag | payloadOffset;
        }

        /// Marks that the represented object's destructor has completed.
        static constexpr std::size_t MarkDestroyed(
            std::size_t token
        ) noexcept {
            return token | DestroyedFlag;
        }

        // Token inspection.

        /// Reports whether a token is empty.
        static constexpr bool IsEmpty(
            std::size_t token
        ) noexcept {
            return token == 0U;
        }

        /// Reports whether a token represents shared-reserve storage.
        static constexpr bool IsShared(
            std::size_t token
        ) noexcept {
            return (token & SharedFlag) != 0U;
        }

        /// Reports whether the represented object's destructor has completed.
        static constexpr bool IsDestroyed(
            std::size_t token
        ) noexcept {
            return (token & DestroyedFlag) != 0U;
        }

        /// Returns the zero-based dedicated slot index encoded by a token.
        static constexpr std::size_t DedicatedIndex(
            std::size_t token
        ) noexcept {
            return (token & ValueMask) - 1U;
        }

        /// Returns the shared-reserve payload allocation encoded by a token.
        static constexpr SharedAllocation SharedAllocationFrom(
            std::size_t token
        ) noexcept {
            return SharedAllocation {
                token & ValueMask
            };
        }

    };

} // ESPressio::Memory::Detail
