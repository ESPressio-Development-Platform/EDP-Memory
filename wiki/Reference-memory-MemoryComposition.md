# src/memory/MemoryComposition.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/MemoryComposition.hpp)

## Direct includes

- `ESPressio_System.hpp`

## Documented declarations

### `Domain`

**Classification:** PUBLIC API

Composition domain containing deterministic Memory capabilities.

```cpp
struct Domain final : Framework::Domain {};
```

### `ByteOperations`

**Classification:** PUBLIC API · source access: `public`

Exclusive capability supplying raw byte-copy, move, fill, and comparison operations.

```cpp
struct ByteOperations final : Framework::ExclusiveCapability<Domain> {};
```

### `MemoryResource`

**Classification:** PUBLIC API · source access: `public`

Shared capability supplying one raw aligned memory resource.

```cpp
struct MemoryResource final : Framework::SharedCapability<Domain> {};
```

### `SharedReserveAllocationAlgorithm`

**Classification:** PUBLIC API · source access: `public`

Exclusive capability supplying the shared-reserve suballocation algorithm.

```cpp
struct SharedReserveAllocationAlgorithm final : Framework::ExclusiveCapability<Domain> {};
```

### `SharedReserveAllocationRequirement`

**Classification:** PUBLIC API · source access: `public`

Same-domain Requirement selecting the unique shared-reserve allocation algorithm.

```cpp
using SharedReserveAllocationRequirement = Framework::Requirement<
        SharedReserveAllocationAlgorithm,
```

### `MemoryResourceRequirement`

**Classification:** PUBLIC API · source access: `public`

Same-domain Requirement selecting all MemoryResource providers.

```cpp
using MemoryResourceRequirement = Framework::Requirement<
        MemoryResource,
```

