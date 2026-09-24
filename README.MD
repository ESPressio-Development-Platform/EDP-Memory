# ESPressio Memory

`EDP-Memory` defines deterministic memory-management contracts and bounded runtime topology for the ESPressio Development Platform.

Its purpose is to make memory cost an application design decision: application Bootstrap declares exact dedicated object capacities, one bounded shared-overflow reserve, the MemoryResource providers supplying those reservations, and the shared-reserve allocation algorithm. Initialization either establishes that exact topology or rolls it back completely.

## Contents

- [Byte Operations and Memory Resources](./docs/BYTE-OPERATIONS-AND-RESOURCES.MD)  
  Defines raw byte-operation and aligned-memory resource contracts, provider ownership boundaries, and allocation/release semantics.

- [Memory Topology](./docs/MEMORY-TOPOLOGY.MD)  
  Defines compile-time Object Pool configuration, resource binding, transactional initialization, topology freeze, and mode-transition teardown.

- [Object Pools and Lifetimes](./docs/OBJECT-POOLS-AND-LIFETIMES.MD)  
  Defines raw-slot reservation, `ObjectPool`, move-only `ObjectPoolLease`, construction/destruction requirements, nested pool guidance, and stable addresses.

- [Shared Reserve Allocation](./docs/SHARED-RESERVE-ALLOCATION.MD)  
  Defines the substitutable shared allocator contract and the default in-band coalescing-first-fit implementation.

- [Concurrency, Waiting and Teardown](./docs/CONCURRENCY-WAITING-AND-TEARDOWN.MD)  
  Defines the topology-wide coordination lock, stack-resident targeted wait requests, explicit wait policy, cancellation, ISR boundary, and complete teardown rules.

## Core Facilities

```text
ByteOperations                     raw copy / move / fill / compare abstraction
MemoryResource                     shared raw aligned allocation capability
SharedReserveAllocationAlgorithm   substitutable bounded suballocator capability
MemoryTopology                     compile-time immutable topology declaration
ObjectPool<T,...>                  dedicated-first typed object capacity
ObjectPoolLease<T,...>             move-only RAII ownership token
ObjectPoolable<T>                  optional zero-state acquisition convenience
MemoryRuntime                      synchronous initialization, waiting, cancellation, teardown
CoalescingFirstFitProvider         default bounded shared-reserve allocator
```

`EDP-Memory` does not implement platform-specific system allocation APIs. Concrete ownership follows the API actually used: Standard C/C++ implementations belong to `EDP-Platform-Portable`; ESP-IDF-specific heap/PSRAM implementations belong to `EDP-Platform-ESP-IDF`; other Platform repositories provide Memory concretes only when the underlying API genuinely belongs there.

## Dependencies

```text
EDP-Memory
├── EDP-System [mandatory]
└── EDP-Platform [mandatory]
    └── EDP-System [mandatory, transitive]
```

There are no optional ESPressio-library dependencies.

Arduino IDE / Arduino CLI consumers discover the mandatory EDP-Platform dependency through its public `<ESPressio_Platform.hpp>` umbrella. Public EDP-Memory headers do not reach directly into another ESPressio library's internal `src/<subdirectory>/...` paths, because doing so bypasses Arduino's recursive library dependency discovery.


See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD) for the maintained dependency graph and provider-direction notes.

## Determinism

- Topology is declared at compile time.
- Dedicated capacities are exact and are never silently reduced.
- The shared reserve is one fixed configured byte region; its configured bytes include allocator metadata and alignment padding.
- Pooled construction performs no second EDP-Memory allocation after a slot/shared span has been claimed.
- Live objects are never relocated or compacted.
- Waiters allocate no heap state; their temporary request and Signal live on the blocked caller's existing stack.
- Production runtime stores no historical diagnostics solely for developer telemetry.

## Lifecycle

```text
Uninitialized
    -> Initializing
    -> InitializedFrozen
    -> TearingDown
    -> Uninitialized
```

Initialization is synchronous, sequential, and all-or-nothing. A configured topology that cannot be established is an application-bootstrap fatal condition, but `EDP-Memory` itself returns strongly typed results and does not log, abort, or reboot.

Complete teardown is first-class. It refuses while live pooled objects remain. Once all consumers have shut down and all leases have been released, one topology may be destroyed completely and another compile-time topology may be initialized, supporting bounded mode transitions such as normal operation to OTA-specific operation.

## Testing

Host coverage validates the default shared allocator, pool lifetime, quotas, waiting/cancellation, rollback, teardown/reinitialize, compact lease representation, and compile-fail topology/lifetime constraints. Platform concrete providers maintain their own provider-specific coverage in their owning repositories.

The host runner executes functional and compile-fail coverage without sanitizers by default. Optional sanitizer passes are selected explicitly through `EDP_MEMORY_SANITIZER_MODE`:

```bash
EDP_MEMORY_SANITIZER_MODE=undefined ./tests/run_host_tests.sh
EDP_MEMORY_SANITIZER_MODE=address ./tests/run_host_tests.sh
EDP_MEMORY_SANITIZER_MODE=address,undefined ./tests/run_host_tests.sh
```

This keeps functional validation independent from host-toolchain sanitizer-runtime availability while retaining explicit ASan/UBSan coverage where the installed compiler/runtime supports it.

## C++ Language Baseline

ESPressio requires C++20. PlatformIO-compatible ESP32 demos use the maintained PIOArduino platform package so Arduino-ESP32 and ESP-IDF builds use a current C++20-capable toolchain baseline.
