# Public API

Public vocabulary includes memory capability contracts, `MemoryTopology`, object-pool specifications, `ObjectPool<T,...>`, move-only `ObjectPoolLease`, compact `ObjectPool::DedicatedIndex` ownership, optional `ObjectPoolable`, `MemoryRuntime` and the default coalescing first-fit shared allocator.

`ObjectPool::AcquireDedicated` is a dedicated-only, non-waiting ownership path. It publishes one strong one-byte `DedicatedIndex`, never consumes shared overflow, and pairs with `DedicatedObject(index)`, `ResetDedicated(index, ...)`, and `ReleaseDedicated(index)`. Reset reconstructs the live object in place without changing occupancy, index, address, or waiter-visible capacity. The general `Acquire(lease, timeout, ...)` path remains the dedicated-first/shared-capable RAII and waiting model.

Dedicated capacity is exact. Shared-overflow quotas are explicit. A zero per-type shared quota can mean no type-specific count cap while still remaining bounded by the shared byte reserve.

Exact declarations remain authoritative in the exported headers.
