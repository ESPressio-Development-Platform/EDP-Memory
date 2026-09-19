#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include <synchronization/WaitTimeout.hpp>

#include "MemoryTypes.hpp"
#include "ObjectPoolLease.hpp"
#include "detail/ObjectPoolState.hpp"
#include "detail/ObjectPoolToken.hpp"

namespace ESPressio::Memory {

    /// Consumer-facing pool for one object Type within a concrete MemoryRuntime.
    ///
    /// The pool owns only bounded backing/occupancy metadata. Application code receives a
    /// non-owning reference to this object from MemoryRuntime::ObjectPoolFor<TObject>().
    ///
    /// @tparam TObject Object Type managed by this pool.
    /// @tparam TObjectPoolSpec Compile-time specification governing this pool.
    /// @tparam TMemoryRuntime Concrete MemoryRuntime that coordinates this pool.
    template<class TObject, class TObjectPoolSpec, class TMemoryRuntime>
    class ObjectPool final : private Detail::DedicatedObjectPoolState<
        TObject,
        TObjectPoolSpec::Dedicated::Value
    >, private Detail::SharedObjectPoolQuotaState<
        typename TObjectPoolSpec::Shared
    > {

        private:

            // Object Pool Type metadata.

            /// Dedicated slot state for this Object Pool.
            using DedicatedState = Detail::DedicatedObjectPoolState<
                TObject,
                TObjectPoolSpec::Dedicated::Value
            >;

            /// Shared quota state for this Object Pool.
            using SharedQuotaState = Detail::SharedObjectPoolQuotaState<
                typename TObjectPoolSpec::Shared
            >;

            // Runtime ownership.

            /// Non-owning pointer to the lifetime-stable MemoryRuntime coordinating this pool.
            TMemoryRuntime* _runtime = nullptr;

            /// Binds this facade to its lifetime-stable owning MemoryRuntime during runtime construction.
            void BindRuntime(
                TMemoryRuntime& runtime
            ) noexcept {
                _runtime = &runtime;
            }

            /// Allows MemoryRuntime to initialize and coordinate private pool state.
            friend TMemoryRuntime;

        public:

            // Construction and lifetime.

            /// Creates an unbound Object Pool facade for internal tuple construction.
            ObjectPool() noexcept = default;

            /// Move-only ownership Type returned by successful acquisition from this pool.
            using LeaseType = ObjectPoolLease<TObject, ObjectPool>;

            /// Object Type managed by this pool.
            using ObjectType = TObject;

            /// Compile-time ObjectPoolSpec selected for this object Type.
            using Spec = TObjectPoolSpec;

            /// Prevents copying pool runtime state.
            ObjectPool(const ObjectPool&) = delete;

            /// Prevents copy assignment of pool runtime state.
            ObjectPool& operator =(const ObjectPool&) = delete;

            /// Prevents moving pool runtime state after references may have escaped to consumers.
            ObjectPool(ObjectPool&&) = delete;

            /// Prevents move assignment of pool runtime state.
            ObjectPool& operator =(ObjectPool&&) = delete;


            // Object acquisition.

            /// Acquires and constructs one object according to the explicitly supplied wait policy.
            ///
            /// @tparam TArguments Constructor argument Types forwarded to TObject.
            /// @param lease Empty output lease that receives ownership only on success.
            /// @param timeout Explicit caller-selected wait policy.
            /// @param arguments Constructor arguments forwarded after capacity is reserved.
            template<class... TArguments>
            ObjectPoolAcquisitionResult Acquire(
                LeaseType& lease,
                ESPressio::Platform::Synchronization::WaitTimeout timeout,
                TArguments&&... arguments
            ) noexcept {
                static_assert(
                    std::is_nothrow_constructible_v<TObject, TArguments...>,
                    "ObjectPool acquisition requires the selected constructor to be noexcept"
                );

                if (_runtime == nullptr) {
                    return ObjectPoolAcquisitionResult::NotInitialized;
                }

                return _runtime->template AcquireObject<TObject>(
                    *this,
                    lease,
                    timeout,
                    std::forward<TArguments>(arguments)...
                );
            }

        private:

            // MemoryRuntime coordination.

            /// Initializes this pool's exact dedicated backing allocation.
            template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider& resource
            ) noexcept {
                return DedicatedState::InitializeDedicated(resource);
            }

            /// Releases this pool's complete dedicated backing allocation.
            template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider& resource
            ) noexcept {
                return DedicatedState::ReleaseDedicatedBacking(resource);
            }

            /// Attempts to claim one dedicated slot and returns its encoded ownership token.
            Detail::ObjectPoolCapacityClaimResult TryClaimDedicated(
                std::size_t& token
            ) noexcept {
                std::size_t slotIndex = 0U;
                const auto result = DedicatedState::TryClaimDedicated(slotIndex);

                if (result != Detail::ObjectPoolCapacityClaimResult::Claimed) {
                    return result;
                }

                token = Detail::ObjectPoolToken::Dedicated(slotIndex);
                return Detail::ObjectPoolCapacityClaimResult::Claimed;
            }

            /// Reports whether a shared allocation may be attempted under this pool's policy/quota.
            bool CanClaimShared() const noexcept {
                return SharedQuotaState::CanClaimShared();
            }

            /// Records one successful shared allocation against this pool's finite quota when configured.
            void ClaimShared() noexcept {
                SharedQuotaState::ClaimShared();
            }

            /// Records release of one shared allocation against this pool's finite quota when configured.
            void ReleaseShared() noexcept {
                SharedQuotaState::ReleaseShared();
            }

            /// Returns one claimed dedicated slot to the vacant set.
            void ReleaseDedicated(
                std::size_t token
            ) noexcept {
                DedicatedState::ReleaseDedicated(
                    Detail::ObjectPoolToken::DedicatedIndex(token)
                );
            }


            /// Publishes one newly constructed object into the caller's empty lease.
            void AdoptLease(
                LeaseType& lease,
                std::size_t token
            ) noexcept {
                lease.Adopt(
                    *this,
                    token
                );
            }

            /// Resolves one live object from the compact lease token.
            TObject* ResolveToken(
                std::size_t token
            ) noexcept {
                return _runtime->template ResolveObject<TObject>(
                    *this,
                    token
                );
            }

            /// Resolves one live object from the compact lease token for const access.
            const TObject* ResolveToken(
                std::size_t token
            ) const noexcept {
                return _runtime->template ResolveObject<TObject>(
                    *this,
                    token
                );
            }

            /// Destroys and releases the object represented by one lease token.
            ObjectPoolLeaseReleaseResult ReleaseToken(
                std::size_t& token
            ) noexcept {
                return _runtime->template ReleaseObject<TObject>(
                    *this,
                    token
                );
            }

            /// Resolves the address associated with one dedicated token.
            TObject* DedicatedAddress(
                std::size_t token
            ) noexcept {
                return DedicatedState::DedicatedAddress(
                    Detail::ObjectPoolToken::DedicatedIndex(token)
                );
            }

            /// Reports whether any dedicated slot remains claimed.
            bool HasLiveDedicatedObjects() const noexcept {
                return DedicatedState::HasLiveDedicatedObjects();
            }

            friend class ObjectPoolLease<TObject, ObjectPool>;

    };

} // ESPressio::Memory
