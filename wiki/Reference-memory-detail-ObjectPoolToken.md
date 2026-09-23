# src/memory/detail/ObjectPoolToken.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/detail/ObjectPoolToken.hpp)

## Direct includes

- `cstddef`
- `limits`
- `../MemoryTypes.hpp`

## Documented declarations

### `ObjectPoolToken`

**Classification:** PRIVATE IMPLEMENTATION

Encodes dedicated/shared object identity and release phase into one machine word.

```cpp
struct ObjectPoolToken final
```

### `SharedFlag`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

High bit identifying a shared-reserve allocation.

```cpp
static constexpr std::size_t SharedFlag =
            std::size_t{1U} << (std::numeric_limits<std::size_t>::digits - 1U);
```

### `DestroyedFlag`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Next-highest bit identifying an object whose destructor has completed but whose storage is not yet returned.

```cpp
static constexpr std::size_t DestroyedFlag =
            std::size_t{1U} << (std::numeric_limits<std::size_t>::digits - 2U);
```

### `ValueMask`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Mask containing the dedicated slot ordinal or shared payload offset.

```cpp
static constexpr std::size_t ValueMask = ~(SharedFlag | DestroyedFlag);
```

### `Empty`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Creates an empty token.

```cpp
static constexpr std::size_t Empty() noexcept
```

### `Dedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Encodes one zero-based dedicated slot index.

```cpp
static constexpr std::size_t Dedicated(
            std::size_t slotIndex
        ) noexcept
```

### `Shared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Encodes one shared-reserve payload offset.

```cpp
static constexpr std::size_t Shared(
            std::size_t payloadOffset
        ) noexcept
```

### `MarkDestroyed`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Marks that the represented object's destructor has completed.

```cpp
static constexpr std::size_t MarkDestroyed(
            std::size_t token
        ) noexcept
```

### `IsEmpty`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether a token is empty.

```cpp
static constexpr bool IsEmpty(
            std::size_t token
        ) noexcept
```

### `IsShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether a token represents shared-reserve storage.

```cpp
static constexpr bool IsShared(
            std::size_t token
        ) noexcept
```

### `IsDestroyed`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether the represented object's destructor has completed.

```cpp
static constexpr bool IsDestroyed(
            std::size_t token
        ) noexcept
```

### `DedicatedIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the zero-based dedicated slot index encoded by a token.

```cpp
static constexpr std::size_t DedicatedIndex(
            std::size_t token
        ) noexcept
```

### `SharedAllocationFrom`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns the shared-reserve payload allocation encoded by a token.

```cpp
static constexpr SharedAllocation SharedAllocationFrom(
            std::size_t token
        ) noexcept
```

