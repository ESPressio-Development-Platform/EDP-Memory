# Build, Test and Source Map

C++20 is required. `docs/` describes topology, pools, shared allocation, concurrency and lifecycle. Tests should protect exact capacity, rollback, contiguous-vs-total failure, waiter ordering, construction/destruction boundaries and teardown refusal with live leases.


Arduino-facing source validation must also verify that public headers discover mandatory sibling libraries through their public root headers. The 2026-09-24 Primitive-introduction tranche specifically guards EDP-Platform discovery from EDP-Memory without adding a new dependency edge.

## Event V1 indexed-pool coverage

`tests/host/main.cpp` validates compact one-byte `DedicatedIndex` layout, dedicated-only no-wait acquisition, coexistence with ordinary shared-capable leases, release/index invalidation, unowned-slot rejection, teardown, and lifetime accounting. `tests/compile_fail/throwing_dedicated_constructor.cpp` proves the indexed path rejects throwing constructors.

`tests/run_host_tests.sh` now requires the sibling EDP-BoundedTopology source tree and exercises the same coverage under GCC/Clang plus explicit sanitizer modes. Representative GitHub Actions mirror that dependency topology.
