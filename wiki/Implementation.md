# Private Implementation

Initialization is all-or-nothing in the order dedicated pools, shared raw reserve, allocator initialization, then frozen publication. Ordinary failure rolls back earlier reservations; rollback failure enters an explicit failed lifecycle.

Object construction/destruction occurs outside the topology mutex after capacity has been reserved. Shared allocation uses first-fit with immediate coalescing and distinguishes total-capacity exhaustion from lack of one contiguous extent.

Compact indexed dedicated acquisition claims only `DedicatedObjectPoolState`, releases the mutex, placement-constructs the object, then publishes a `BoundedIndex`. Indexed release verifies occupancy under the mutex, destroys outside it, returns capacity under the mutex, and runs the ordinary waiter service so general Object Pool waiters remain coherent with indexed consumers.

If synchronization fails after indexed destruction, Memory poisons coordination and invalidates the caller index rather than exposing a path that could invoke the destructor twice.
