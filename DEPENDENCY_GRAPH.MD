# EDP-Memory Dependency Graph

> Revalidated: 2026-09-23 — Composition Framework enhancement migration changes compile-time provider/requirement metadata and validation only; no ESPressio-library dependency edge changed.

## Library Dependency Tree

```text
EDP-Memory
├── EDP-System [mandatory]
└── EDP-Platform [mandatory]
    └── EDP-System [mandatory, transitive]
```

## Mandatory Dependencies

- `EDP-System` — provides the Composition Framework used to define Memory capabilities and validate application-selected providers.
- `EDP-Platform` — provides the Mutex, Signal, result, and `WaitTimeout` contracts used by concurrent Object Pool acquisition, targeted waiting, cancellation, and teardown coordination.

## Optional Dependencies

`EDP-Memory` has no optional ESPressio-library dependencies.

Concrete providers which satisfy `EDP-Memory` capabilities are selected by application Bootstrap and therefore depend **toward** this repository rather than being dependencies of it. Standard C/C++ memory-operation providers are owned by `EDP-Platform-Portable`; ESP-IDF heap-capability / PSRAM providers are owned by `EDP-Platform-ESP-IDF`.
