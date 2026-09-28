# Internal API

Internal machinery owns dedicated-slot bitmaps/indices, in-band shared-allocation metadata, waiter linkage, topology-wide bookkeeping, rollback state and shutdown cancellation. Wait requests are intrusive on the blocked caller's stack rather than permanently allocated per pool.

The indexed dedicated path reuses the same occupancy bitmap and topology coordination mutex. `TryClaimDedicatedIndex`, `IsDedicatedOccupied`, `DedicatedAddressAt` and `ReleaseDedicatedIndex` are private MemoryRuntime coordination seams; they are not consumer ownership APIs.
