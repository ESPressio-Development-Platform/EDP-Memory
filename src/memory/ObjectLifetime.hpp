#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace ESPressio::Memory {

    /// Deterministic typed object-lifetime operations for caller-supplied storage.
    ///
    /// This facility does not allocate storage. It centralizes construction, ownership-transfer
    /// construction, and destruction so consuming EDP domains do not directly perform C++
    /// lifetime primitives or std::move across the Memory abstraction boundary.
    struct ObjectLifetime final {

        /// Constructs TObject in caller-supplied suitably aligned storage.
        template<class TObject, class... TArguments>
        [[nodiscard]]
        static TObject* Construct(
            void* storage,
            TArguments&&... arguments
        ) noexcept {
            static_assert(
                std::is_nothrow_constructible_v<TObject, TArguments...>,
                "ObjectLifetime::Construct requires the selected constructor to be noexcept"
            );

            return std::construct_at(
                static_cast<TObject*>(storage),
                std::forward<TArguments>(arguments)...
            );
        }

        /// Move-constructs TObject in caller-supplied suitably aligned destination storage.
        ///
        /// The source object remains live in a valid moved-from state. Its owner remains
        /// responsible for destroying it exactly once.
        template<class TObject>
        [[nodiscard]]
        static TObject* MoveConstruct(
            void* destination,
            TObject& source
        ) noexcept {
            static_assert(
                std::is_nothrow_move_constructible_v<TObject>,
                "ObjectLifetime::MoveConstruct requires TObject to be nothrow move constructible"
            );

            return std::construct_at(
                static_cast<TObject*>(destination),
                std::move(source)
            );
        }

        /// Destroys one live TObject without releasing or otherwise modifying its storage.
        template<class TObject>
        static void Destroy(
            TObject& object
        ) noexcept {
            static_assert(
                std::is_nothrow_destructible_v<TObject>,
                "ObjectLifetime::Destroy requires TObject to be nothrow destructible"
            );

            std::destroy_at(&object);
        }

    };

} // ESPressio::Memory
