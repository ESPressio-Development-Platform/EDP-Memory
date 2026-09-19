# Object Pools and Lifetimes

## Raw slots, not preconstructed objects

An Object Pool reserves storage capable of containing its configured instances. Unused slots do not contain live objects.

Acquisition claims capacity first, then placement-constructs the requested object in that exact storage. EDP-Memory does not perform a second dynamic allocation to instantiate the object after capacity has been claimed.

The selected constructor must be `noexcept`, and the pooled Type must be nothrow destructible. This gives deterministic lifetime transitions without exception rollback machinery.

A Type does not need a default constructor; `Acquire` forwards explicit constructor arguments.

## Dedicated first, shared second

For a Type configured with both dedicated capacity and shared overflow, acquisition order is fixed:

1. first vacant dedicated slot;
2. shared reserve if eligible/quota permits;
3. caller-selected wait behavior when neither source can satisfy the request.

This ordering is not configurable in v1. Dedicated slots are the guaranteed baseline; shared storage is elastic overflow.

## Stable addresses

Once construction completes, the object address remains stable until release. Dedicated objects remain in their slot; shared objects remain in their allocation. The shared allocator may split/coalesce free regions but never relocates live objects.

## ObjectPoolLease

Successful acquisition publishes move-only `ObjectPoolLease` ownership. The lease stores only:

- a non-owning Object Pool pointer;
- one compact machine-word token encoding dedicated/shared origin and release phase.

It deliberately does not cache `T*`. Resolving the address from pool + token has trivial deterministic execution cost and saves one machine word per live lease.

The lease:

- is default-empty;
- is non-copyable and movable;
- releases automatically in its `noexcept` destructor;
- supports explicit `.Release()` for early capacity return;
- exposes `Get`, `operator*`, and `operator->` as non-owning object access while ownership is live;
- has no `Detach()`/abandon-ownership operation.

For long-lived scopes, explicitly call `.Release()` as soon as the object is no longer needed rather than waiting for scope exit. This returns scarce preallocated capacity sooner.

## Nested object guidance

If pooled `A` contains `B` directly by value, `sizeof(A)` already contains the storage for `B`; separately leasing B would add unnecessary indirection/capacity dependency.

If A requires a separately-owned B that would otherwise require dynamic allocation, B should itself seek deterministic preallocation / pooling. Prefer acquiring the child lease before constructing the parent where practical.

Application developers must right-size dependent pools and avoid circular capacity waits. EDP-Memory intentionally does not add a runtime dependency solver/deadlock detector.

## Teardown ownership rule

Memory teardown never forcibly destroys arbitrary live pooled objects in v1. Higher-level domains must stop their work and release all leases first. If live objects remain, teardown returns `LiveObjectsRemain`.

This makes shutdown ordering explicit and prevents Memory from silently invalidating references or bypassing domain-specific lifecycle behavior.
