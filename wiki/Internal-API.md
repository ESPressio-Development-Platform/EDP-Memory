# Internal API

Internal machinery owns dedicated-slot bitmaps/indices, in-band shared-allocation metadata, waiter linkage, topology-wide bookkeeping, rollback state and shutdown cancellation. Wait requests are intrusive on the blocked caller's stack rather than permanently allocated per pool.
