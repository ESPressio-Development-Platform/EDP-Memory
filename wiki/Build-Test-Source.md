# Build, Test and Source Map

C++20 is required. `docs/` describes topology, pools, shared allocation, concurrency and lifecycle. Tests should protect exact capacity, rollback, contiguous-vs-total failure, waiter ordering, construction/destruction boundaries and teardown refusal with live leases.


Arduino-facing source validation must also verify that public headers discover mandatory sibling libraries through their public root headers. The 2026-09-24 Primitive-introduction tranche specifically guards EDP-Platform discovery from EDP-Memory without adding a new dependency edge.

## Event V1 indexed-pool coverage

`tests/host/main.cpp` validates compact one-byte `DedicatedIndex` layout, dedicated-only no-wait acquisition, in-place reset with stable index/address/occupancy, provider/topology/ownership reset failures, coexistence with ordinary shared-capable leases, reset not servicing blocked waiters, release/index invalidation, unowned-slot rejection, teardown, and lifetime accounting. Compile-fail coverage proves both indexed acquisition and indexed reset reject throwing selected constructors.

`tests/run_host_tests.sh` now requires the sibling EDP-BoundedTopology source tree and exercises the same coverage under GCC/Clang plus explicit sanitizer modes. Representative GitHub Actions mirror that dependency topology.

## 9 November 2026 — native demo concrete-provider selection

The five `demos/` families use public library-root includes so Arduino IDE/CLI discovers the required concrete repositories: `ESPressio_Platform_Portable_MemoryResource.hpp` for the existing Standard C++ aligned resource, `ESPressio_Platform_FreeRTOS.hpp` for ESP32 Mutex/Signal (when required), and `ESPressio_Platform_Portable_ByteOperations.hpp` for raw byte operations. Direct PlatformIO manifests name these providers, with no reliance on a transitive dependency to select application concretes. The broad Portable umbrella is not a drop-in substitute: its AtomicWord32 and Portable Synchronization impose separate lock-free and exception contracts. Validate each of the Arduino IDE, PlatformIO Arduino and PlatformIO ESP-IDF variants independently; a host test or application compile is not automatically hardware execution. Direct native finite ObjectPool wait remains unqualified under all failure modes ([Memory #16](https://github.com/ESPressio-Development-Platform/EDP-Memory/issues/16)); the Logging NoWait producer profile avoids that path.
