# Memory Topology

## Compile-time application plan

`MemoryTopology` is the immutable compile-time declaration of an application's deterministic Memory layout.

Each topology selects:

- one default `MemoryResource` provider for dedicated Object Pools;
- one `SharedReserve<N, Provider>`;
- an ordered set of `ObjectPoolSpec<T,...>` declarations.

Each object Type may appear at most once. Duplicate Object Pool specifications are rejected at compile time.

Pool configuration belongs to application Bootstrap, never to the reusable object Type.

## Dedicated capacity

`DedicatedInstances<N>` declares the exact guaranteed dedicated capacity for one Type. N is constrained to the locked one-byte range 0..255.

A topology requesting N dedicated instances either reserves storage for exactly N instances or initialization fails. EDP-Memory never silently reduces configured capacity.

Dedicated Object Pools are reserved synchronously in topology declaration order. This makes placement priority deterministic without requiring a second runtime priority mechanism.

## Shared overflow policy

Shared reserve eligibility is explicit:

- `NoSharedOverflow` — the Type can never consume shared reserve;
- `SharedOverflow<0>` — shared overflow is permitted with no per-Type simultaneous-instance quota;
- `SharedOverflow<N>`, N > 0 — shared overflow is permitted for at most N simultaneous shared-backed instances.

A zero shared quota therefore means "uncapped by per-Type count", not "unbounded memory": the single shared reserve remains a fixed byte budget.

## Resource selection

A topology identifies concrete `MemoryResource` providers selected by Bootstrap. A pool can use the topology default or override it with `UseMemoryResource<Provider>`.

The selected provider Type must be present in the Memory Composition. There is no runtime resource registry or service lookup.

V1 has exactly one shared reserve and one backing `MemoryResource` provider for that reserve. If a provider implements external-preferred/internal-fallback placement, that fallback remains inside the provider.

## Initialization order and atomicity

`MemoryRuntime::Initialize()` is synchronous and sequential:

1. reserve every dedicated Object Pool in declaration order;
2. reserve the configured shared byte region;
3. initialize the selected shared-reserve allocator over that region;
4. publish the topology as `InitializedFrozen`.

Initialization is transactionally all-or-nothing. Any ordinary failure unwinds every reservation established during that attempt and returns the runtime to `Uninitialized`.

If rollback itself fails because a provider cannot release an earlier reservation, the runtime enters `InitializationFailed` and reports `RollbackFailed`. The application should treat this as a fatal provider/bootstrap condition; EDP-Memory itself does not log, abort, or reboot.

`MemoryTopologyInitializationFailure` provides transient context, including the failing Object Pool ordinal when applicable, without retaining Type names or a runtime diagnostic registry.

## Frozen topology

After successful initialization the following cannot change:

- pool Types;
- dedicated capacities;
- dedicated backing locations;
- shared eligibility/quotas;
- shared reserve base/extent;
- resource provider bindings;
- shared allocator provider.

Only occupancy changes as objects are constructed and released.

There is no resize, growth, shrink, rebalance, migration, compaction, provider replacement, or algorithm replacement while initialized.

## Complete teardown

Complete teardown is first-class, but it is not mutation of an initialized topology.

Higher domains first stop their work and release every `ObjectPoolLease`. Memory then verifies that no pending acquisition and no live object/shared allocation remains before releasing the shared reserve and dedicated backing blocks.

A successful teardown returns the runtime to `Uninitialized`. The same topology or a different compile-time topology may subsequently be initialized, supporting bounded operating-mode transitions such as normal application operation to an OTA-specific memory layout.

Provider-release failure during teardown enters `TeardownFailed`; EDP-Memory never reports a clean teardown when resource ownership is uncertain.
