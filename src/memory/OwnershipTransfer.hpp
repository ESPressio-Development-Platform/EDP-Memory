#pragma once

#include <type_traits>

namespace ESPressio::Memory {

    /// Marks an explicit typed ownership transfer across an EDP domain boundary.
    ///
    /// This is intentionally a Memory-domain operation: consumers express semantic transfer
    /// without reaching directly for the C++ library move primitive. Actual object lifetime
    /// establishment remains the responsibility of ObjectLifetime.
    struct OwnershipTransfer final {

        template<class TObject>
        [[nodiscard]]
        static constexpr std::remove_reference_t<TObject>&& Move(
            TObject&& object
        ) noexcept {
            using Object = std::remove_reference_t<TObject>;
            return static_cast<Object&&>(object);
        }

    };

} // ESPressio::Memory
