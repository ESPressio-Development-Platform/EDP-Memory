#pragma once

#include <type_traits>
#include <utility>

namespace ESPressio::Memory {

    /// Optional zero-state CRTP convenience for Types commonly constructed through ObjectPool.
    ///
    /// This helper performs no hidden lookup and stores no pool binding. The caller must still
    /// supply the exact ObjectPool and wait policy explicitly at each acquisition site.
    ///
    /// @tparam TDerived Concrete pooled Type exposing this convenience surface.
    template<class TDerived>
    class ObjectPoolable {

        protected:

            // Construction.

            /// Allows only derived Types to construct this zero-state convenience base.
            ObjectPoolable() = default;

            /// Allows derived Types to use ordinary copy construction semantics for this empty base.
            ObjectPoolable(const ObjectPoolable&) = default;

            /// Allows derived Types to use ordinary move construction semantics for this empty base.
            ObjectPoolable(ObjectPoolable&&) = default;

            /// Allows derived Types to use ordinary copy assignment semantics for this empty base.
            ObjectPoolable& operator =(const ObjectPoolable&) = default;

            /// Allows derived Types to use ordinary move assignment semantics for this empty base.
            ObjectPoolable& operator =(ObjectPoolable&&) = default;

            /// Provides protected destruction because this base is not a polymorphic ownership boundary.
            ~ObjectPoolable() = default;

        public:

            // Explicit pool construction convenience.

            /// Acquires one TDerived from the supplied ObjectPool using the explicit caller wait policy.
            ///
            /// @tparam TObjectPool Concrete ObjectPool managing TDerived.
            /// @tparam TWaitTimeout Platform wait-policy Type accepted by TObjectPool.
            /// @tparam TArguments Constructor argument Types forwarded to TDerived.
            /// @param pool Explicit pool selected by the caller or Bootstrap wiring.
            /// @param lease Empty output lease that receives ownership only on success.
            /// @param timeout Explicit caller-selected wait policy.
            /// @param arguments Constructor arguments forwarded to TDerived.
            template<
                class TObjectPool,
                class TWaitTimeout,
                class... TArguments
            >
            static auto FromPool(
                TObjectPool& pool,
                typename TObjectPool::LeaseType& lease,
                TWaitTimeout timeout,
                TArguments&&... arguments
            ) noexcept -> decltype(
                pool.Acquire(
                    lease,
                    timeout,
                    std::forward<TArguments>(arguments)...
                )
            ) {
                static_assert(
                    std::is_same_v<typename TObjectPool::ObjectType, TDerived>,
                    "ObjectPoolable::FromPool requires an ObjectPool for the derived Type"
                );

                return pool.Acquire(
                    lease,
                    timeout,
                    std::forward<TArguments>(arguments)...
                );
            }

    };

} // ESPressio::Memory
