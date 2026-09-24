# Build, Test and Source Map

C++20 is required. `docs/` describes topology, pools, shared allocation, concurrency and lifecycle. Tests should protect exact capacity, rollback, contiguous-vs-total failure, waiter ordering, construction/destruction boundaries and teardown refusal with live leases.


Arduino-facing source validation must also verify that public headers discover mandatory sibling libraries through their public root headers. The 2026-09-24 Primitive-introduction tranche specifically guards EDP-Platform discovery from EDP-Memory without adding a new dependency edge.
