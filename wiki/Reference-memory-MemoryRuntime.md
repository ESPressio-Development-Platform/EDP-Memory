# src/memory/MemoryRuntime.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `68ba27e5cfc30fdbc43237c4c08557e79f25c5d3`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/68ba27e5cfc30fdbc43237c4c08557e79f25c5d3/src/memory/MemoryRuntime.hpp)

## Direct includes

- `ESPressio_Platform.hpp`
- `cstddef`
- `new`
- `tuple`
- `type_traits`
- `utility`
- `MemoryComposition.hpp`
- `MemoryResourceContract.hpp`
- `MemoryTopology.hpp`
- `MemoryTypes.hpp`
- `ObjectPool.hpp`
- `SharedReserveAllocationContract.hpp`
- `detail/ObjectPoolToken.hpp`
- `detail/TopologyTraits.hpp`
- `detail/WaitRequest.hpp`

## Documented declarations

### `TTopology`

**Classification:** PUBLIC API

Concrete runtime implementation specialized from one MemoryResource provider list and ObjectPoolSpec pack.

```cpp
template<
        class TTopology,
```

### `TTopology`

**Classification:** PUBLIC API

MemoryRuntime specialization containing the complete compile-time resource and pool packs.

```cpp
template<
        class TTopology,
```

### `Self`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Concrete self Type used to bind ObjectPool facades.

```cpp
using Self = MemoryRuntimeImpl;
```

### `SharedAllocatorProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Shared-reserve allocation provider selected by the Memory Composition.

```cpp
using SharedAllocatorProvider = typename TMemoryComposition::template Select<
                SharedReserveAllocationRequirement,
```

### `MutexContract`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Platform Mutex contract selected for topology coordination.

```cpp
using MutexContract = ESPressio::Platform::Synchronization::Detail::MutexProviderTraits<TMutexProvider>;
```

### `SignalContract`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Platform Signal contract selected for targeted waiter notification.

```cpp
using SignalContract = ESPressio::Platform::Synchronization::Detail::SignalProviderTraits<TSignalProvider>;
```

### `SharedAllocatorContract`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Shared allocator contract selected by the Memory Composition.

```cpp
using SharedAllocatorContract = SharedReserveAllocationProviderTraits<SharedAllocatorProvider>;
```

### `ObjectPoolTuple`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Ordered tuple of consumer-facing Object Pools.

```cpp
using ObjectPoolTuple = std::tuple<
                ObjectPool<typename TObjectPoolSpecs::Object, TObjectPoolSpecs, Self>...
            >;
```

### `WaitRequestType`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Intrusive wait request Type used only while a caller is blocked.

```cpp
using WaitRequestType = WaitRequest<TSignalProvider>;
```

### `_mutex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Lifetime-stable topology coordination mutex supplied by Bootstrap.

```cpp
TMutexProvider* _mutex;
```

### `_sharedAllocator`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Lifetime-stable shared-reserve allocation algorithm supplied by Bootstrap.

```cpp
SharedAllocatorProvider* _sharedAllocator;
```

### `_resources`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Lifetime-stable MemoryResource providers supplied by Bootstrap in Composition order.

```cpp
std::tuple<TMemoryResourceProviders*...> _resources;
```

### `ObjectPoolTuple _objectPools{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Consumer-facing Object Pools in declaration order.

```cpp
ObjectPoolTuple _objectPools{};
```

### `MemoryBlock _sharedReserveBlock{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Raw backing block reserved for the one shared overflow region.

```cpp
MemoryBlock _sharedReserveBlock{};
```

### `_waitHead`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First pending waiter in arrival order.

```cpp
WaitRequestType* _waitHead = nullptr;
```

### `_waitTail`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Last pending waiter in arrival order.

```cpp
WaitRequestType* _waitTail = nullptr;
```

### `_state`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Current topology lifecycle state.

```cpp
MemoryTopologyState _state = MemoryTopologyState::Uninitialized;
```

### `_acquisitionsCancelled`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates that pending/new acquisitions have been cancelled for shutdown.

```cpp
bool _acquisitionsCancelled = false;
```

### `_coordinationFailed`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates that a targeted notification provider failed while capacity was being handed off.

```cpp
bool _coordinationFailed = false;
```

### `BindObjectPools`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Binds every default-constructed ObjectPool facade to this lifetime-stable runtime.

```cpp
template<std::size_t... TIndices>
            void BindObjectPools(
                std::index_sequence<TIndices...>
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves one Bootstrap-owned MemoryResource provider by concrete Type.

```cpp
template<class TMemoryResourceProvider>
            TMemoryResourceProvider& Resource() noexcept
```

### `AcquireCoordinationLock`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Acquires the topology coordination mutex indefinitely.

```cpp
ESPressio::Platform::Synchronization::LockAcquireResult AcquireCoordinationLock() noexcept
```

### `ReleaseCoordinationLock`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Releases the topology coordination mutex.

```cpp
ESPressio::Platform::Synchronization::LockReleaseResult ReleaseCoordinationLock() noexcept
```

### `AppendWaitRequest`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Appends one stack-resident wait request in arrival order.

```cpp
void AppendWaitRequest(
                WaitRequestType& request
            ) noexcept
```

### `RemoveWaitRequest`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Removes one wait request from the intrusive arrival-order list.

```cpp
void RemoveWaitRequest(
                WaitRequestType& request
            ) noexcept
```

### `TObjectPool`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Attempts one shared allocation for a pool whose policy permits it.

```cpp
template<class TObjectPool>
            ObjectPoolCapacityClaimResult TryClaimSharedLocked(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept
```

### `TObjectPool`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Attempts the locked dedicated-first acquisition policy for one Object Pool.

```cpp
template<class TObjectPool>
            ObjectPoolCapacityClaimResult TryClaimLocked(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept
```

### `TObjectPool`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Releases a claimed token that has not had an object constructed into it.

```cpp
template<class TObjectPool>
            ObjectPoolCapacityReturnResult ReturnUnconstructedCapacityLocked(
                TObjectPool& pool,
                std::size_t token
            ) noexcept
```

### `TryClaimByIndexLocked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Dispatches a capacity claim by runtime ObjectPoolSpec ordinal.

```cpp
template<std::size_t TIndex = 0U>
            ObjectPoolCapacityClaimResult TryClaimByIndexLocked(
                std::size_t objectPoolIndex,
                std::size_t& token
            ) noexcept
```

### `ReturnUnconstructedByIndexLocked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Dispatches return of an unconstructed reservation by ObjectPoolSpec ordinal.

```cpp
template<std::size_t TIndex = 0U>
            ObjectPoolCapacityReturnResult ReturnUnconstructedByIndexLocked(
                std::size_t objectPoolIndex,
                std::size_t token
            ) noexcept
```

### `ServiceWaitersLocked`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Grants capacity to waiting requests in oldest-satisfiable order.

```cpp
WaitRequestServiceResult ServiceWaitersLocked() noexcept
```

### `InitializeDedicatedPools`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Initializes every dedicated Object Pool sequentially in topology declaration order.

```cpp
template<std::size_t TIndex = 0U>
            MemoryTopologyInitializationResult InitializeDedicatedPools(
                MemoryTopologyInitializationFailure& failure
            ) noexcept
```

### `ReleaseDedicatedPoolsReverse`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Releases every dedicated backing block in reverse declaration order.

```cpp
template<std::size_t TCount = sizeof...(TObjectPoolSpecs)>
            MemoryReleaseResult ReleaseDedicatedPoolsReverse() noexcept
```

### `HasLiveDedicatedObjects`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether any dedicated Object Pool still owns a live or in-construction slot.

```cpp
template<std::size_t TIndex = 0U>
            bool HasLiveDedicatedObjects() const noexcept
```

### `Topology`

**Classification:** PUBLIC API · source access: `public`

Complete compile-time topology represented by this runtime.

```cpp
using Topology = TTopology;
```

### `Composition`

**Classification:** PUBLIC API · source access: `public`

Complete compile-time Memory Composition represented by this runtime.

```cpp
using Composition = TMemoryComposition;
```

### `SharedAllocator`

**Classification:** PUBLIC API · source access: `public`

Shared allocator provider Type selected by the Memory Composition.

```cpp
using SharedAllocator = SharedAllocatorProvider;
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

ObjectPoolSpec configured for TObject.

```cpp
template<class TObject>
            using ObjectPoolSpecFor = typename TTopology::template ObjectPoolSpecFor<TObject>;
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Consumer-facing ObjectPool Type configured for TObject.

```cpp
template<class TObject>
            using ObjectPoolType = ObjectPool<TObject, ObjectPoolSpecFor<TObject>, Self>;
```

### `MemoryRuntimeImpl`

**Classification:** PUBLIC API · source access: `public`

Creates an uninitialized Memory runtime from Bootstrap-owned provider instances.

```cpp
MemoryRuntimeImpl(
                TMutexProvider& mutex,
                SharedAllocatorProvider& sharedAllocator,
                TMemoryResourceProviders&... resources
            ) noexcept :
                _mutex(&mutex),
```

### `MemoryRuntimeImpl`

**Classification:** PUBLIC API · source access: `public`

Prevents copying lifetime-stable Memory runtime state.

```cpp
MemoryRuntimeImpl(const MemoryRuntimeImpl&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents copy assignment of lifetime-stable Memory runtime state.

```cpp
MemoryRuntimeImpl& operator =(const MemoryRuntimeImpl&) = delete;
```

### `MemoryRuntimeImpl`

**Classification:** PUBLIC API · source access: `public`

Prevents moving runtime state after pool references may have escaped.

```cpp
MemoryRuntimeImpl(MemoryRuntimeImpl&&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents move assignment of runtime state.

```cpp
MemoryRuntimeImpl& operator =(MemoryRuntimeImpl&&) = delete;
```

### `Initialize`

**Classification:** PUBLIC API · source access: `public`

Synchronously establishes the complete configured topology or rolls back every reservation.

```cpp
MemoryTopologyInitializationResult Initialize(
                MemoryTopologyInitializationFailure& failure
            ) noexcept
```

### `CancelPendingAcquisitions`

**Classification:** PUBLIC API · source access: `public`

Cancels pending waiters and prevents all subsequent acquisitions until complete teardown/reinitialization.

```cpp
PendingAcquisitionCancellationResult CancelPendingAcquisitions() noexcept
```

### `TearDown`

**Classification:** PUBLIC API · source access: `public`

Tears down the complete topology only when no pending acquisition or live pooled object remains.

```cpp
MemoryTopologyTeardownResult TearDown() noexcept
```

### `State`

**Classification:** PUBLIC API · source access: `public`

Returns the current lifecycle state of this Memory topology.

```cpp
MemoryTopologyState State() const noexcept
```

### `IsInitialized`

**Classification:** PUBLIC API · source access: `public`

Reports whether this topology is fully initialized and frozen for normal operation.

```cpp
bool IsInitialized() const noexcept
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Returns the one Object Pool configured for TObject.

```cpp
template<class TObject>
            ObjectPoolType<TObject>& ObjectPoolFor() noexcept
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Returns the one Object Pool configured for TObject for const access.

```cpp
template<class TObject>
            const ObjectPoolType<TObject>& ObjectPoolFor() const noexcept
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Acquires, constructs, and publishes one TObject into an output lease.

```cpp
template<
                class TObject,
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves one live pooled object address from its compact lease token.

```cpp
template<class TObject, class TObjectPool>
            TObject* ResolveObject(
                TObjectPool& pool,
                std::size_t token
            ) noexcept
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves one live pooled object address for const access.

```cpp
template<class TObject, class TObjectPool>
            const TObject* ResolveObject(
                const TObjectPool& pool,
                std::size_t token
            ) const noexcept
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Destroys one pooled object outside the coordination lock and then returns its capacity.

```cpp
template<class TObject, class TObjectPool>
            ObjectPoolLeaseReleaseResult ReleaseObject(
                TObjectPool& pool,
                std::size_t& token
            ) noexcept
```

### `TTopology`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Runtime owner for one compile-time Memory topology.

Bootstrap selects the Memory Composition and concrete Platform synchronization providers,
owns the provider instances, and supplies them explicitly to this runtime.

- **Template parameter `TTopology`:** Compile-time immutable Memory topology.
- **Template parameter `TMemoryComposition`:** Compile-time Memory capability provider selection.
- **Template parameter `TMutexProvider`:** Concrete Platform Mutex provider used for topology coordination.
- **Template parameter `TSignalProvider`:** Concrete Platform Signal provider instantiated per blocked waiter.

```cpp
template<
        class TTopology,
```

