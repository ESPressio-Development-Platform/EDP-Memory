# Public API

Public vocabulary includes memory capability contracts, `MemoryTopology`, object-pool specifications, `ObjectPool<T,...>`, move-only `ObjectPoolLease`, optional `ObjectPoolable`, `MemoryRuntime` and the default coalescing first-fit shared allocator.

Dedicated capacity is exact. Shared-overflow quotas are explicit. A zero per-type shared quota can mean no type-specific count cap while still remaining bounded by the shared byte reserve.

Exact declarations remain authoritative in the exported headers.
