# src/memory/ObjectPoolConfiguration.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/ObjectPoolConfiguration.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `type_traits`

## Documented declarations

### `DedicatedInstances`

**Classification:** PUBLIC API

Declares the exact dedicated instance count reserved for one pooled Type.

```cpp
template<std::size_t TCount>
    struct DedicatedInstances final
```

### `Value`

**Classification:** PUBLIC API · source access: `public`

Exact dedicated instance count.

```cpp
static constexpr std::uint8_t Value = static_cast<std::uint8_t>(TCount);
```

### `NoSharedOverflow`

**Classification:** PUBLIC API

Declares that one pooled Type may never consume the shared reserve.

```cpp
struct NoSharedOverflow final
```

### `IsEnabled`

**Classification:** PUBLIC API · source access: `public`

Indicates that shared-reserve allocation is disabled for this Type.

```cpp
static constexpr bool IsEnabled = false;
```

### `MaximumInstances`

**Classification:** PUBLIC API · source access: `public`

Maximum simultaneous shared instances; unused when shared overflow is disabled.

```cpp
static constexpr std::uint8_t MaximumInstances = 0U;
```

### `SharedOverflow`

**Classification:** PUBLIC API

Declares shared-reserve eligibility and an optional per-Type simultaneous-instance quota.

```cpp
template<std::size_t TMaximumInstances>
    struct SharedOverflow final
```

### `IsEnabled`

**Classification:** PUBLIC API · source access: `public`

Indicates that shared-reserve allocation is enabled for this Type.

```cpp
static constexpr bool IsEnabled = true;
```

### `MaximumInstances`

**Classification:** PUBLIC API · source access: `public`

Maximum simultaneous shared instances, or zero for no per-Type quota.

```cpp
static constexpr std::uint8_t MaximumInstances = static_cast<std::uint8_t>(TMaximumInstances);
```

### `UseDefaultMemoryResource`

**Classification:** PUBLIC API

Selects the topology-wide default MemoryResource for one Object Pool.

```cpp
struct UseDefaultMemoryResource final {};
```

### `TMemoryResourceProvider`

**Classification:** PUBLIC API · source access: `public`

Selects one explicit MemoryResource provider for one Object Pool.

- **Template parameter `TMemoryResourceProvider`:** Concrete MemoryResource provider selected by Bootstrap.

```cpp
template<class TMemoryResourceProvider>
    struct UseMemoryResource final
```

### `Provider`

**Classification:** PUBLIC API · source access: `public`

Concrete MemoryResource provider Type used by this Object Pool.

```cpp
using Provider = TMemoryResourceProvider;
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Describes one pooled Type and its deterministic dedicated/shared capacity policy.

- **Template parameter `TObject`:** Object Type whose instances are managed by this pool.
- **Template parameter `TDedicatedInstances`:** DedicatedInstances declaration for TObject.
- **Template parameter `TSharedOverflow`:** Shared overflow policy for TObject.
- **Template parameter `TMemoryResourceSelection`:** Default or explicit dedicated MemoryResource selection.

```cpp
template<
        class TObject,
```

### `Object`

**Classification:** PUBLIC API · source access: `public`

Object Type managed by this pool.

```cpp
using Object = TObject;
```

### `Dedicated`

**Classification:** PUBLIC API · source access: `public`

Dedicated capacity declaration.

```cpp
using Dedicated = TDedicatedInstances;
```

### `Shared`

**Classification:** PUBLIC API · source access: `public`

Shared overflow policy.

```cpp
using Shared = TSharedOverflow;
```

### `ResourceSelection`

**Classification:** PUBLIC API · source access: `public`

Dedicated MemoryResource selection.

```cpp
using ResourceSelection = TMemoryResourceSelection;
```

### `TBytes`

**Classification:** PUBLIC API · source access: `public`

Describes the one bounded shared reserve used by a Memory topology.

- **Template parameter `TBytes`:** Absolute configured reserve bytes, including allocator metadata and padding.
- **Template parameter `TMemoryResourceProvider`:** Concrete MemoryResource provider backing the reserve.

```cpp
template<
        std::size_t TBytes,
```

### `Bytes`

**Classification:** PUBLIC API · source access: `public`

Absolute configured shared-reserve byte count.

```cpp
static constexpr std::size_t Bytes = TBytes;
```

### `ResourceProvider`

**Classification:** PUBLIC API · source access: `public`

Concrete MemoryResource provider backing the shared reserve.

```cpp
using ResourceProvider = TMemoryResourceProvider;
```

