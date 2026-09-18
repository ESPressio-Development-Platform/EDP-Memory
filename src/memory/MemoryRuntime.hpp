#pragma once

#include <cstddef>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>

#include <synchronization/SynchronizationContracts.hpp>
#include <synchronization/SynchronizationTypes.hpp>
#include <synchronization/WaitTimeout.hpp>

#include "MemoryComposition.hpp"
#include "MemoryResourceContract.hpp"
#include "MemoryTopology.hpp"
#include "MemoryTypes.hpp"
#include "ObjectPool.hpp"
#include "SharedReserveAllocationContract.hpp"
#include "detail/ObjectPoolToken.hpp"
#include "detail/TopologyTraits.hpp"
#include "detail/WaitRequest.hpp"

namespace ESPressio::Memory::Detail {

    /// Concrete runtime implementation specialized from one MemoryResource provider list and ObjectPoolSpec pack.
    template<
        class TTopology,
        class TMemoryComposition,
        class TMutexProvider,
        class TSignalProvider,
        class TResourceProviderList,
        class TObjectPoolSpecTuple
    >
    class MemoryRuntimeImpl;


    /// MemoryRuntime specialization containing the complete compile-time resource and pool packs.
    template<
        class TTopology,
        class TMemoryComposition,
        class TMutexProvider,
        class TSignalProvider,
        class... TMemoryResourceProviders,
        class... TObjectPoolSpecs
    >
    class MemoryRuntimeImpl<
        TTopology,
        TMemoryComposition,
        TMutexProvider,
        TSignalProvider,
        ESPressio::System::CompositionFramework::ProviderList<TMemoryResourceProviders...>,
        std::tuple<TObjectPoolSpecs...>
    > final {

        private:

            /// Concrete self Type used to bind ObjectPool facades.
            using Self = MemoryRuntimeImpl;

            /// Shared-reserve allocation provider selected by the Memory Composition.
            using SharedAllocatorProvider = typename TMemoryComposition::template ProviderFor<SharedReserveAllocationAlgorithm>;

            /// Platform Mutex contract selected for topology coordination.
            using MutexContract = ESPressio::Platform::Synchronization::Detail::MutexProviderTraits<TMutexProvider>;

            /// Platform Signal contract selected for targeted waiter notification.
            using SignalContract = ESPressio::Platform::Synchronization::Detail::SignalProviderTraits<TSignalProvider>;

            /// Shared allocator contract selected by the Memory Composition.
            using SharedAllocatorContract = SharedReserveAllocationProviderTraits<SharedAllocatorProvider>;

            /// Ordered tuple of consumer-facing Object Pools.
            using ObjectPoolTuple = std::tuple<
                ObjectPool<typename TObjectPoolSpecs::Object, TObjectPoolSpecs, Self>...
            >;

            /// Intrusive wait request Type used only while a caller is blocked.
            using WaitRequestType = WaitRequest<TSignalProvider>;

            static_assert(
                std::is_same_v<typename TMemoryComposition::CompositionDomain, Domain>,
                "MemoryRuntime requires a Composition for the EDP-Memory Domain"
            );

            static_assert(
                sizeof(MutexContract) > 0U &&
                sizeof(SignalContract) > 0U &&
                sizeof(SharedAllocatorContract) > 0U,
                "MemoryRuntime provider contracts must be complete"
            );

            static_assert(
                ((sizeof(MemoryResourceProviderTraits<TMemoryResourceProviders>) > 0U) && ...),
                "Every MemoryResource provider must satisfy the EDP-Memory resource contract"
            );

            static_assert(
                TTopology::SharedReserveSpec::Bytes <= ObjectPoolToken::ValueMask,
                "Shared reserve is too large for the compact ObjectPool lease token"
            );

            static_assert(
                ProviderListContains<
                    typename TTopology::DefaultMemoryResourceProvider,
                    ESPressio::System::CompositionFramework::ProviderList<TMemoryResourceProviders...>
                >::value,
                "Topology default MemoryResource provider must be present in the Memory Composition"
            );

            static_assert(
                ProviderListContains<
                    typename TTopology::SharedReserveSpec::ResourceProvider,
                    ESPressio::System::CompositionFramework::ProviderList<TMemoryResourceProviders...>
                >::value,
                "Shared reserve MemoryResource provider must be present in the Memory Composition"
            );

            static_assert(
                (
                    ProviderListContains<
                        typename TTopology::template ResourceProviderFor<TObjectPoolSpecs>,
                        ESPressio::System::CompositionFramework::ProviderList<TMemoryResourceProviders...>
                    >::value && ...
                ),
                "Every ObjectPool dedicated MemoryResource provider must be present in the Memory Composition"
            );

            // Bootstrap-selected providers.

            /// Lifetime-stable topology coordination mutex supplied by Bootstrap.
            TMutexProvider* _mutex;

            /// Lifetime-stable shared-reserve allocation algorithm supplied by Bootstrap.
            SharedAllocatorProvider* _sharedAllocator;

            /// Lifetime-stable MemoryResource providers supplied by Bootstrap in Composition order.
            std::tuple<TMemoryResourceProviders*...> _resources;

            // Topology state.

            /// Consumer-facing Object Pools in declaration order.
            ObjectPoolTuple _objectPools{};

            /// Raw backing block reserved for the one shared overflow region.
            MemoryBlock _sharedReserveBlock{};

            /// First pending waiter in arrival order.
            WaitRequestType* _waitHead = nullptr;

            /// Last pending waiter in arrival order.
            WaitRequestType* _waitTail = nullptr;

            /// Current topology lifecycle state.
            MemoryTopologyState _state = MemoryTopologyState::Uninitialized;

            /// Indicates that pending/new acquisitions have been cancelled for shutdown.
            bool _acquisitionsCancelled = false;

            /// Indicates that a targeted notification provider failed while capacity was being handed off.
            bool _coordinationFailed = false;


            // Runtime binding.

            /// Binds every default-constructed ObjectPool facade to this lifetime-stable runtime.
            template<std::size_t... TIndices>
            void BindObjectPools(
                std::index_sequence<TIndices...>
            ) noexcept {
                (
                    std::get<TIndices>(_objectPools).BindRuntime(*this),
                    ...
                );
            }

            /// Resolves one Bootstrap-owned MemoryResource provider by concrete Type.
            template<class TMemoryResourceProvider>
            TMemoryResourceProvider& Resource() noexcept {
                return *std::get<TMemoryResourceProvider*>(_resources);
            }


            // Coordination lock.

            /// Acquires the topology coordination mutex indefinitely.
            bool AcquireCoordinationLock() noexcept {
                return _mutex->Acquire(
                    ESPressio::Platform::Synchronization::WaitTimeout::Forever()
                ) == ESPressio::Platform::Synchronization::LockAcquireResult::Acquired;
            }

            /// Releases the topology coordination mutex.
            bool ReleaseCoordinationLock() noexcept {
                return _mutex->Release() == ESPressio::Platform::Synchronization::LockReleaseResult::Released;
            }


            // Wait-list management.

            /// Appends one stack-resident wait request in arrival order.
            void AppendWaitRequest(
                WaitRequestType& request
            ) noexcept {
                request.Previous = _waitTail;
                request.Next = nullptr;

                if (_waitTail != nullptr) {
                    _waitTail->Next = &request;
                } else {
                    _waitHead = &request;
                }

                _waitTail = &request;
            }

            /// Removes one wait request from the intrusive arrival-order list.
            void RemoveWaitRequest(
                WaitRequestType& request
            ) noexcept {
                if (request.Previous != nullptr) {
                    request.Previous->Next = request.Next;
                } else if (_waitHead == &request) {
                    _waitHead = request.Next;
                }

                if (request.Next != nullptr) {
                    request.Next->Previous = request.Previous;
                } else if (_waitTail == &request) {
                    _waitTail = request.Previous;
                }

                request.Previous = nullptr;
                request.Next = nullptr;
            }


            // Shared reserve allocation.

            /// Attempts one shared allocation for a pool whose policy permits it.
            template<class TObjectPool>
            bool TryClaimSharedLocked(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept {
                using TObject = typename TObjectPool::ObjectType;

                if (!pool.CanClaimShared()) { return false; }

                SharedAllocation allocation{};
                const auto result = _sharedAllocator->Allocate(
                    sizeof(TObject),
                    alignof(TObject),
                    allocation
                );

                if (result != SharedAllocationResult::Succeeded) { return false; }

                if (
                    allocation.PayloadOffset == 0U ||
                    allocation.PayloadOffset > ObjectPoolToken::ValueMask
                ) {
                    static_cast<void>(
                        _sharedAllocator->Release(allocation)
                    );
                    return false;
                }

                pool.ClaimShared();
                token = ObjectPoolToken::Shared(allocation.PayloadOffset);

                return true;
            }

            /// Attempts the locked dedicated-first acquisition policy for one Object Pool.
            template<class TObjectPool>
            bool TryClaimLocked(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept {
                if (pool.TryClaimDedicated(token)) { return true; }

                if constexpr (TObjectPool::Spec::Shared::IsEnabled) {
                    return TryClaimSharedLocked(
                        pool,
                        token
                    );
                }

                return false;
            }

            /// Releases a claimed token that has not had an object constructed into it.
            template<class TObjectPool>
            void ReturnUnconstructedCapacityLocked(
                TObjectPool& pool,
                std::size_t token
            ) noexcept {
                if (ObjectPoolToken::IsShared(token)) {
                    const auto allocation = ObjectPoolToken::SharedAllocationFrom(token);

                    if (
                        _sharedAllocator->Release(allocation) == SharedAllocationReleaseResult::Released
                    ) {
                        pool.ReleaseShared();
                    }

                    return;
                }

                pool.ReleaseDedicated(token);
            }

            /// Dispatches a capacity claim by runtime ObjectPoolSpec ordinal.
            template<std::size_t TIndex = 0U>
            bool TryClaimByIndexLocked(
                std::size_t objectPoolIndex,
                std::size_t& token
            ) noexcept {
                if constexpr (TIndex >= sizeof...(TObjectPoolSpecs)) {
                    return false;
                } else {
                    if (objectPoolIndex == TIndex) {
                        return TryClaimLocked(
                            std::get<TIndex>(_objectPools),
                            token
                        );
                    }

                    return TryClaimByIndexLocked<TIndex + 1U>(
                        objectPoolIndex,
                        token
                    );
                }
            }

            /// Dispatches return of an unconstructed reservation by ObjectPoolSpec ordinal.
            template<std::size_t TIndex = 0U>
            void ReturnUnconstructedByIndexLocked(
                std::size_t objectPoolIndex,
                std::size_t token
            ) noexcept {
                if constexpr (TIndex < sizeof...(TObjectPoolSpecs)) {
                    if (objectPoolIndex == TIndex) {
                        ReturnUnconstructedCapacityLocked(
                            std::get<TIndex>(_objectPools),
                            token
                        );
                        return;
                    }

                    ReturnUnconstructedByIndexLocked<TIndex + 1U>(
                        objectPoolIndex,
                        token
                    );
                }
            }

            /// Grants capacity to waiting requests in oldest-satisfiable order.
            void ServiceWaitersLocked() noexcept {
                auto* request = _waitHead;

                while (request != nullptr) {
                    auto* next = request->Next;
                    std::size_t token = ObjectPoolToken::Empty();

                    if (
                        request->State == WaitRequestState::Waiting &&
                        TryClaimByIndexLocked(
                            request->ObjectPoolIndex,
                            token
                        )
                    ) {
                        request->Token = token;
                        request->State = WaitRequestState::Granted;
                        RemoveWaitRequest(*request);

                        const auto notifyResult = request->Signal.Notify();

                        if (notifyResult != ESPressio::Platform::Synchronization::SignalNotifyResult::Signaled) {
                            request->State = WaitRequestState::Waiting;
                            request->Token = ObjectPoolToken::Empty();
                            ReturnUnconstructedByIndexLocked(
                                request->ObjectPoolIndex,
                                token
                            );
                            AppendWaitRequest(*request);
                            _coordinationFailed = true;
                        }
                    }

                    request = next;
                }
            }


            // Dedicated backing lifecycle.

            /// Initializes every dedicated Object Pool sequentially in topology declaration order.
            template<std::size_t TIndex = 0U>
            MemoryTopologyInitializationResult InitializeDedicatedPools(
                MemoryTopologyInitializationFailure& failure
            ) noexcept {
                if constexpr (TIndex >= sizeof...(TObjectPoolSpecs)) {
                    return MemoryTopologyInitializationResult::Succeeded;
                } else {
                    using Spec = std::tuple_element_t<TIndex, std::tuple<TObjectPoolSpecs...>>;
                    using ResourceProvider = typename TTopology::template ResourceProviderFor<Spec>;

                    auto& pool = std::get<TIndex>(_objectPools);
                    auto& resource = Resource<ResourceProvider>();
                    const auto result = pool.InitializeDedicated(resource);

                    if (result != MemoryAllocationResult::Succeeded) {
                        failure.Result = result == MemoryAllocationResult::ProviderFailure
                            ? MemoryTopologyInitializationResult::ProviderFailure
                            : MemoryTopologyInitializationResult::DedicatedReservationFailed;
                        failure.ObjectPoolIndex = TIndex;
                        failure.ResourceResult = result;
                        return failure.Result;
                    }

                    return InitializeDedicatedPools<TIndex + 1U>(failure);
                }
            }

            /// Releases every dedicated backing block in reverse declaration order.
            template<std::size_t TCount = sizeof...(TObjectPoolSpecs)>
            bool ReleaseDedicatedPoolsReverse() noexcept {
                if constexpr (TCount == 0U) {
                    return true;
                } else {
                    constexpr std::size_t index = TCount - 1U;
                    using Spec = std::tuple_element_t<index, std::tuple<TObjectPoolSpecs...>>;
                    using ResourceProvider = typename TTopology::template ResourceProviderFor<Spec>;

                    auto& pool = std::get<index>(_objectPools);
                    auto& resource = Resource<ResourceProvider>();
                    const auto result = pool.ReleaseDedicatedBacking(resource);

                    if (result != MemoryReleaseResult::Released) { return false; }

                    return ReleaseDedicatedPoolsReverse<index>();
                }
            }

            /// Reports whether any dedicated Object Pool still owns a live or in-construction slot.
            template<std::size_t TIndex = 0U>
            bool HasLiveDedicatedObjects() const noexcept {
                if constexpr (TIndex >= sizeof...(TObjectPoolSpecs)) {
                    return false;
                } else {
                    if (std::get<TIndex>(_objectPools).HasLiveDedicatedObjects()) { return true; }

                    return HasLiveDedicatedObjects<TIndex + 1U>();
                }
            }

        public:

            /// Complete compile-time topology represented by this runtime.
            using Topology = TTopology;

            /// Complete compile-time Memory Composition represented by this runtime.
            using Composition = TMemoryComposition;

            /// Shared allocator provider Type selected by the Memory Composition.
            using SharedAllocator = SharedAllocatorProvider;

            /// ObjectPoolSpec configured for TObject.
            template<class TObject>
            using ObjectPoolSpecFor = typename TTopology::template ObjectPoolSpecFor<TObject>;

            /// Consumer-facing ObjectPool Type configured for TObject.
            template<class TObject>
            using ObjectPoolType = ObjectPool<TObject, ObjectPoolSpecFor<TObject>, Self>;

            // Construction and lifetime.

            /// Creates an uninitialized Memory runtime from Bootstrap-owned provider instances.
            MemoryRuntimeImpl(
                TMutexProvider& mutex,
                SharedAllocatorProvider& sharedAllocator,
                TMemoryResourceProviders&... resources
            ) noexcept :
                _mutex(&mutex),
                _sharedAllocator(&sharedAllocator),
                _resources(&resources...) {
                BindObjectPools(
                    std::make_index_sequence<sizeof...(TObjectPoolSpecs)>{}
                );
            }

            /// Prevents copying lifetime-stable Memory runtime state.
            MemoryRuntimeImpl(const MemoryRuntimeImpl&) = delete;

            /// Prevents copy assignment of lifetime-stable Memory runtime state.
            MemoryRuntimeImpl& operator =(const MemoryRuntimeImpl&) = delete;

            /// Prevents moving runtime state after pool references may have escaped.
            MemoryRuntimeImpl(MemoryRuntimeImpl&&) = delete;

            /// Prevents move assignment of runtime state.
            MemoryRuntimeImpl& operator =(MemoryRuntimeImpl&&) = delete;


            // Topology lifecycle.

            /// Synchronously establishes the complete configured topology or rolls back every reservation.
            MemoryTopologyInitializationResult Initialize(
                MemoryTopologyInitializationFailure& failure
            ) noexcept {
                if (_state != MemoryTopologyState::Uninitialized) {
                    return MemoryTopologyInitializationResult::AlreadyInitialized;
                }

                _state = MemoryTopologyState::Initializing;
                _acquisitionsCancelled = false;
                _coordinationFailed = false;
                failure = MemoryTopologyInitializationFailure{};

                const auto dedicatedResult = InitializeDedicatedPools(failure);

                if (dedicatedResult != MemoryTopologyInitializationResult::Succeeded) {
                    if (!ReleaseDedicatedPoolsReverse()) {
                        _state = MemoryTopologyState::InitializationFailed;
                        failure.Result = MemoryTopologyInitializationResult::RollbackFailed;
                        return failure.Result;
                    }

                    _state = MemoryTopologyState::Uninitialized;
                    return dedicatedResult;
                }

                if constexpr (TTopology::SharedReserveSpec::Bytes > 0U) {
                    using SharedResourceProvider = typename TTopology::SharedReserveSpec::ResourceProvider;

                    auto& sharedResource = Resource<SharedResourceProvider>();
                    MemoryBlock candidate{};
                    const auto allocationResult = sharedResource.Allocate(
                        TTopology::SharedReserveSpec::Bytes,
                        alignof(std::max_align_t),
                        candidate
                    );

                    if (allocationResult != MemoryAllocationResult::Succeeded) {
                        failure.Result = allocationResult == MemoryAllocationResult::ProviderFailure
                            ? MemoryTopologyInitializationResult::ProviderFailure
                            : MemoryTopologyInitializationResult::SharedReserveReservationFailed;
                        failure.ObjectPoolIndex = MemoryTopologyInitializationFailure::NoObjectPoolIndex;
                        failure.ResourceResult = allocationResult;

                        if (!ReleaseDedicatedPoolsReverse()) {
                            _state = MemoryTopologyState::InitializationFailed;
                            failure.Result = MemoryTopologyInitializationResult::RollbackFailed;
                            return failure.Result;
                        }

                        _state = MemoryTopologyState::Uninitialized;
                        return failure.Result;
                    }

                    _sharedReserveBlock = candidate;

                    MemoryBlock boundedReserve = candidate;
                    boundedReserve.Size = TTopology::SharedReserveSpec::Bytes;

                    const auto allocatorResult = _sharedAllocator->Initialize(boundedReserve);

                    if (allocatorResult != SharedReserveInitializationResult::Succeeded) {
                        const auto sharedReleaseResult = sharedResource.Release(_sharedReserveBlock);
                        const auto dedicatedRollbackSucceeded = ReleaseDedicatedPoolsReverse();

                        if (
                            sharedReleaseResult != MemoryReleaseResult::Released ||
                            !dedicatedRollbackSucceeded
                        ) {
                            _state = MemoryTopologyState::InitializationFailed;
                            failure.Result = MemoryTopologyInitializationResult::RollbackFailed;
                            return failure.Result;
                        }

                        _sharedReserveBlock = MemoryBlock{};
                        _state = MemoryTopologyState::Uninitialized;
                        failure.Result = MemoryTopologyInitializationResult::AllocatorInitializationFailed;
                        return failure.Result;
                    }
                }

                _state = MemoryTopologyState::InitializedFrozen;
                return MemoryTopologyInitializationResult::Succeeded;
            }

            /// Cancels pending waiters and prevents all subsequent acquisitions until complete teardown/reinitialization.
            PendingAcquisitionCancellationResult CancelPendingAcquisitions() noexcept {
                if (_state != MemoryTopologyState::InitializedFrozen) {
                    return PendingAcquisitionCancellationResult::NotInitialized;
                }

                if (_acquisitionsCancelled) {
                    return PendingAcquisitionCancellationResult::AlreadyCancelled;
                }

                if (!AcquireCoordinationLock()) {
                    return PendingAcquisitionCancellationResult::ProviderFailure;
                }

                _acquisitionsCancelled = true;
                bool notificationFailed = false;
                auto* request = _waitHead;

                while (request != nullptr) {
                    auto* next = request->Next;
                    request->State = WaitRequestState::Cancelled;
                    RemoveWaitRequest(*request);

                    if (
                        request->Signal.Notify() !=
                        ESPressio::Platform::Synchronization::SignalNotifyResult::Signaled
                    ) {
                        notificationFailed = true;
                    }

                    request = next;
                }

                const auto released = ReleaseCoordinationLock();

                if (!released || notificationFailed) {
                    _coordinationFailed = true;
                    return PendingAcquisitionCancellationResult::ProviderFailure;
                }

                return PendingAcquisitionCancellationResult::Cancelled;
            }

            /// Tears down the complete topology only when no pending acquisition or live pooled object remains.
            MemoryTopologyTeardownResult TearDown() noexcept {
                if (_state == MemoryTopologyState::Uninitialized) {
                    return MemoryTopologyTeardownResult::NotInitialized;
                }

                if (_state != MemoryTopologyState::InitializedFrozen) {
                    return MemoryTopologyTeardownResult::TopologyUnavailable;
                }

                if (!AcquireCoordinationLock()) {
                    return MemoryTopologyTeardownResult::TopologyUnavailable;
                }

                if (_waitHead != nullptr) {
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );
                    return MemoryTopologyTeardownResult::PendingAcquisitionsRemain;
                }

                if (
                    HasLiveDedicatedObjects() ||
                    (
                        TTopology::SharedReserveSpec::Bytes > 0U &&
                        _sharedAllocator->HasLiveAllocations()
                    )
                ) {
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );
                    return MemoryTopologyTeardownResult::LiveObjectsRemain;
                }

                _state = MemoryTopologyState::TearingDown;
                static_cast<void>(
                    ReleaseCoordinationLock()
                );

                if constexpr (TTopology::SharedReserveSpec::Bytes > 0U) {
                    if (_sharedAllocator->TearDown() != SharedAllocatorTeardownResult::Succeeded) {
                        _state = MemoryTopologyState::TeardownFailed;
                        return MemoryTopologyTeardownResult::AllocatorTeardownFailed;
                    }

                    using SharedResourceProvider = typename TTopology::SharedReserveSpec::ResourceProvider;
                    auto& sharedResource = Resource<SharedResourceProvider>();

                    if (sharedResource.Release(_sharedReserveBlock) != MemoryReleaseResult::Released) {
                        _state = MemoryTopologyState::TeardownFailed;
                        return MemoryTopologyTeardownResult::ProviderReleaseFailed;
                    }

                    _sharedReserveBlock = MemoryBlock{};
                }

                if (!ReleaseDedicatedPoolsReverse()) {
                    _state = MemoryTopologyState::TeardownFailed;
                    return MemoryTopologyTeardownResult::ProviderReleaseFailed;
                }

                _state = MemoryTopologyState::Uninitialized;
                _acquisitionsCancelled = false;
                _coordinationFailed = false;

                return MemoryTopologyTeardownResult::Succeeded;
            }


            // Topology inspection.

            /// Returns the current lifecycle state of this Memory topology.
            MemoryTopologyState State() const noexcept {
                return _state;
            }

            /// Reports whether this topology is fully initialized and frozen for normal operation.
            bool IsInitialized() const noexcept {
                return _state == MemoryTopologyState::InitializedFrozen;
            }


            // Object Pool access.

            /// Returns the one Object Pool configured for TObject.
            template<class TObject>
            ObjectPoolType<TObject>& ObjectPoolFor() noexcept {
                static_assert(
                    TTopology::template ContainsObjectPool<TObject>,
                    "ObjectPoolFor<TObject> requires TObject to appear in the MemoryTopology"
                );

                return std::get<TTopology::template ObjectPoolIndex<TObject>>(_objectPools);
            }

            /// Returns the one Object Pool configured for TObject for const access.
            template<class TObject>
            const ObjectPoolType<TObject>& ObjectPoolFor() const noexcept {
                static_assert(
                    TTopology::template ContainsObjectPool<TObject>,
                    "ObjectPoolFor<TObject> requires TObject to appear in the MemoryTopology"
                );

                return std::get<TTopology::template ObjectPoolIndex<TObject>>(_objectPools);
            }

        private:

            template<class, class, class>
            friend class ESPressio::Memory::ObjectPool;

            // Object acquisition.

            /// Acquires, constructs, and publishes one TObject into an output lease.
            template<
                class TObject,
                class TObjectPool,
                class TObjectLease,
                class... TArguments
            >
            ObjectPoolAcquisitionResult AcquireObject(
                TObjectPool& pool,
                TObjectLease& lease,
                ESPressio::Platform::Synchronization::WaitTimeout timeout,
                TArguments&&... arguments
            ) noexcept {
                if (!lease.IsEmpty()) { return ObjectPoolAcquisitionResult::OutputLeaseOccupied; }
                if (_state == MemoryTopologyState::Uninitialized) { return ObjectPoolAcquisitionResult::NotInitialized; }
                if (_state != MemoryTopologyState::InitializedFrozen || _acquisitionsCancelled) {
                    return ObjectPoolAcquisitionResult::TopologyUnavailable;
                }
                if (_coordinationFailed) { return ObjectPoolAcquisitionResult::ProviderFailure; }

                if (!AcquireCoordinationLock()) { return ObjectPoolAcquisitionResult::ProviderFailure; }

                if (_state != MemoryTopologyState::InitializedFrozen || _acquisitionsCancelled) {
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );
                    return ObjectPoolAcquisitionResult::TopologyUnavailable;
                }

                std::size_t token = ObjectPoolToken::Empty();

                if (TryClaimLocked(
                    pool,
                    token
                )) {
                    if (!ReleaseCoordinationLock()) {
                        return ObjectPoolAcquisitionResult::ProviderFailure;
                    }

                    auto* object = ResolveObject<TObject>(
                        pool,
                        token
                    );

                    ::new (static_cast<void*>(object)) TObject(
                        std::forward<TArguments>(arguments)...
                    );

                    pool.AdoptLease(
                        lease,
                        token
                    );

                    return ObjectPoolAcquisitionResult::Succeeded;
                }

                if (timeout.IsNoWait()) {
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );
                    return ObjectPoolAcquisitionResult::CapacityUnavailable;
                }

                WaitRequestType request;
                request.ObjectPoolIndex = TTopology::template ObjectPoolIndex<TObject>;
                AppendWaitRequest(request);

                if (!ReleaseCoordinationLock()) {
                    return ObjectPoolAcquisitionResult::ProviderFailure;
                }

                const auto waitResult = request.Signal.Wait(timeout);

                if (!AcquireCoordinationLock()) {
                    return ObjectPoolAcquisitionResult::ProviderFailure;
                }

                if (request.State == WaitRequestState::Granted) {
                    token = request.Token;
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );

                    auto* object = ResolveObject<TObject>(
                        pool,
                        token
                    );

                    ::new (static_cast<void*>(object)) TObject(
                        std::forward<TArguments>(arguments)...
                    );

                    pool.AdoptLease(
                        lease,
                        token
                    );

                    return ObjectPoolAcquisitionResult::Succeeded;
                }

                if (request.State == WaitRequestState::Cancelled) {
                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );
                    return ObjectPoolAcquisitionResult::TopologyUnavailable;
                }

                RemoveWaitRequest(request);
                static_cast<void>(
                    ReleaseCoordinationLock()
                );

                if (waitResult == ESPressio::Platform::Synchronization::SignalWaitResult::ProviderFailure) {
                    return ObjectPoolAcquisitionResult::ProviderFailure;
                }

                return ObjectPoolAcquisitionResult::TimedOut;
            }

            /// Resolves one live pooled object address from its compact lease token.
            template<class TObject, class TObjectPool>
            TObject* ResolveObject(
                TObjectPool& pool,
                std::size_t token
            ) noexcept {
                if (ObjectPoolToken::IsShared(token)) {
                    return static_cast<TObject*>(
                        _sharedAllocator->AddressOf(
                            ObjectPoolToken::SharedAllocationFrom(token)
                        )
                    );
                }

                return pool.DedicatedAddress(token);
            }

            /// Resolves one live pooled object address for const access.
            template<class TObject, class TObjectPool>
            const TObject* ResolveObject(
                const TObjectPool& pool,
                std::size_t token
            ) const noexcept {
                if (ObjectPoolToken::IsShared(token)) {
                    return static_cast<const TObject*>(
                        const_cast<SharedAllocatorProvider*>(_sharedAllocator)->AddressOf(
                            ObjectPoolToken::SharedAllocationFrom(token)
                        )
                    );
                }

                return const_cast<TObjectPool&>(pool).DedicatedAddress(token);
            }

            /// Destroys one pooled object outside the coordination lock and then returns its capacity.
            template<class TObject, class TObjectPool>
            ObjectPoolLeaseReleaseResult ReleaseObject(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept {
                if (ObjectPoolToken::IsEmpty(token)) { return ObjectPoolLeaseReleaseResult::AlreadyEmpty; }

                if (!ObjectPoolToken::IsDestroyed(token)) {
                    if (!AcquireCoordinationLock()) { return ObjectPoolLeaseReleaseResult::ProviderFailure; }

                    static_cast<void>(
                        ReleaseCoordinationLock()
                    );

                    auto* object = ResolveObject<TObject>(
                        pool,
                        token
                    );

                    object->~TObject();
                    token = ObjectPoolToken::MarkDestroyed(token);
                }

                if (!AcquireCoordinationLock()) { return ObjectPoolLeaseReleaseResult::ProviderFailure; }

                if (ObjectPoolToken::IsShared(token)) {
                    const auto allocation = ObjectPoolToken::SharedAllocationFrom(token);
                    const auto result = _sharedAllocator->Release(allocation);

                    if (result != SharedAllocationReleaseResult::Released) {
                        static_cast<void>(
                            ReleaseCoordinationLock()
                        );
                        return ObjectPoolLeaseReleaseResult::ProviderFailure;
                    }

                    pool.ReleaseShared();
                } else {
                    pool.ReleaseDedicated(token);
                }

                ServiceWaitersLocked();
                const auto released = ReleaseCoordinationLock();

                if (!released) { return ObjectPoolLeaseReleaseResult::ProviderFailure; }

                token = ObjectPoolToken::Empty();
                return ObjectPoolLeaseReleaseResult::Released;
            }

    };

} // ESPressio::Memory::Detail

namespace ESPressio::Memory {

    /// Runtime owner for one compile-time Memory topology.
    ///
    /// Bootstrap selects the Memory Composition and concrete Platform synchronization providers,
    /// owns the provider instances, and supplies them explicitly to this runtime.
    ///
    /// @tparam TTopology Compile-time immutable Memory topology.
    /// @tparam TMemoryComposition Compile-time Memory capability provider selection.
    /// @tparam TMutexProvider Concrete Platform Mutex provider used for topology coordination.
    /// @tparam TSignalProvider Concrete Platform Signal provider instantiated per blocked waiter.
    template<
        class TTopology,
        class TMemoryComposition,
        class TMutexProvider,
        class TSignalProvider
    >
    using MemoryRuntime = Detail::MemoryRuntimeImpl<
        TTopology,
        TMemoryComposition,
        TMutexProvider,
        TSignalProvider,
        typename TMemoryComposition::template ProvidersFor<MemoryResource>,
        typename TTopology::ObjectPoolSpecs
    >;

} // ESPressio::Memory
