# Indexed Dedicated Object Pools

## Purpose

`ObjectPool` supports a compact dedicated-only ownership path for consumers whose pool Type is statically known and whose runtime state already owns the pool reference. This path is designed for bounded domain runtimes such as Event occurrence storage, where retaining a pointer-bearing `ObjectPoolLease` for every live object would duplicate invariant pool identity.

The Memory runtime remains the storage and object-lifetime owner. The consumer retains only a strong `ObjectPool::DedicatedIndex`.

## Compact identity

Each `ObjectPool` exposes:

```cpp
using DedicatedIndex = ESPressio::BoundedTopology::BoundedIndex<...>;
```

The index space is private to that exact Object Pool Type, so unrelated pool indices are not interchangeable. `DedicatedInstances<N>` remains constrained to 0..255, therefore the bounded index occupies exactly one byte: values `0..N-1` identify dedicated slots and `0xFF` is the invalid sentinel.

The index carries no pool pointer, allocation pointer, shared-reserve identity, release-phase word, or dynamic ownership state.

## Acquisition

`AcquireDedicated(index, constructorArguments...)`:

1. requires the output index to be invalid;
2. requires the Memory topology to be initialized and available for acquisition;
3. claims the lowest-index vacant dedicated slot under the existing Memory coordination mutex;
4. never waits and never falls back to shared overflow;
5. releases Memory coordination before invoking the selected `noexcept` constructor;
6. publishes the strong index only after construction completes.

The result is `DedicatedObjectPoolAcquisitionResult` with `Succeeded`, `CapacityUnavailable`, `NotInitialized`, `TopologyUnavailable`, `OutputIndexOccupied`, and `ProviderFailure`.

Because the constructor must be `noexcept`, no exception rollback state is required.

## Object access

`DedicatedObject(index)` resolves the stable object already owned by a valid live dedicated index. It is a precondition-based accessor rather than a second ownership operation: the caller must supply an index returned successfully by the same pool and not yet released.

The address remains stable for the complete indexed ownership lifetime.

## Release

`ReleaseDedicated(index)`:

1. validates that the supplied index is valid and currently owns a claimed dedicated slot;
2. releases the Memory coordination mutex before invoking the nothrow destructor;
3. reacquires coordination, returns the slot to the pool, and services ordinary Object Pool waiters;
4. invalidates the caller index after ownership has ended.

The result is `DedicatedObjectPoolReleaseResult` with `Released`, `InvalidIndex`, `SlotNotOwned`, `NotInitialized`, `TopologyUnavailable`, and `ProviderFailure`.

A synchronization-provider failure after object destruction is a fatal Memory-coordination condition. In that case the caller index is invalidated rather than permitting a second destructor invocation. The runtime reports provider failure and does not pretend the topology remains healthy.

## Relationship to ObjectPoolLease

The ordinary `Acquire(lease, timeout, ...)` path remains unchanged. It is still the general-purpose ownership model when a consumer needs:

- dedicated-first plus optional shared overflow;
- finite or indefinite waiting;
- RAII release tied to a movable ownership object;
- one ownership token that can represent either dedicated or shared backing.

Indexed dedicated ownership is intentionally narrower. It is appropriate only when the consumer already has a stable reference to the exact pool and benefits from retaining one compact slot identity per live object.

## Concurrency, teardown and ISR boundary

Indexed acquisition and release use the same topology-wide ordinary-context mutex and occupancy bookkeeping as the existing pool path. Releasing indexed capacity can therefore wake an already-blocked ordinary pool waiter.

A live indexed slot counts as a live dedicated object, so topology teardown returns `LiveObjectsRemain` until it has been released.

Like the rest of Object Pool runtime behavior, indexed ownership is not ISR-safe and must not be used from interrupt context.
