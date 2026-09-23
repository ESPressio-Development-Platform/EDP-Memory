# Private Implementation

Initialization is all-or-nothing in the order dedicated pools, shared raw reserve, allocator initialization, then frozen publication. Ordinary failure rolls back earlier reservations; rollback failure enters an explicit failed lifecycle.

Object construction/destruction occurs outside the topology mutex after capacity has been reserved. Shared allocation uses first-fit with immediate coalescing and distinguishes total-capacity exhaustion from lack of one contiguous extent.
