# EDP-Memory Dependency Graph

> Revalidated: 2026-09-28 — Event V1 indexed dedicated-pool tranche adds the mandatory `EDP-BoundedTopology` dependency for strong compact dedicated-slot identity. The direction is acyclic because EDP-BoundedTopology has no dependency on EDP-Memory.

## Library Dependency Tree

```text
EDP-Memory
├── EDP-System [mandatory]
├── EDP-Platform [mandatory]
│   └── EDP-System [mandatory, transitive]
└── EDP-BoundedTopology [mandatory]
```

## Mandatory Dependencies

- `EDP-System` — provides the Composition Framework used to define Memory capabilities and validate application-selected providers.
- `EDP-Platform` — provides the Mutex, Signal, result, and `WaitTimeout` contracts used by concurrent Object Pool acquisition, targeted waiting, cancellation, and teardown coordination.
- `EDP-BoundedTopology` — provides `BoundedIndex`, reused directly as the exact compact semantic identity for dedicated Object Pool slots. The dependency is representation-only; BoundedTopology remains non-owning and unsynchronized.

## Optional Dependencies

`EDP-Memory` has no optional ESPressio-library dependencies.

Concrete providers which satisfy `EDP-Memory` capabilities are selected by application Bootstrap and therefore depend **toward** this repository rather than being dependencies of it. Standard C/C++ memory-operation providers are owned by `EDP-Platform-Portable`; ESP-IDF heap-capability / PSRAM providers are owned by `EDP-Platform-ESP-IDF`.

## Circular-dependency audit

The new `EDP-Memory -> EDP-BoundedTopology` edge is acyclic. `EDP-BoundedTopology` currently owns only finite identity/membership/topology values and has no dependency back to EDP-Memory.

## Demo-only concrete provider edges (9 November 2026)

The `demos/` applications are **consumers** of the abstract EDP-Memory library, not new dependencies of EDP-Memory itself. Where each ESP32 demo activates the Standard C++ resource concrete, the **application** directly declares `EDP-Platform-Portable#main` and uses its narrow root `ESPressio_Platform_Portable_MemoryResource.hpp`; where a demo activates native Mutex/Signal, it additionally declares `EDP-Platform-FreeRTOS#main`. Raw ByteOperations demos activate only the Portable narrow ByteOperations entry point. The selected concrete libraries depend **toward** EDP-Memory/Platform, never the reverse. This graph remains acyclic with mandatory System/Platform/BoundedTopology only; no invisible transitive edge or unsupported portable AtomicWord32 contract is imposed by the abstract library.
