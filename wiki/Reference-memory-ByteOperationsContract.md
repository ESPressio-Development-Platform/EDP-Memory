# src/memory/ByteOperationsContract.hpp

**Primary classification:** INTERNAL PROVIDER API

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/ByteOperationsContract.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `type_traits`
- `utility`
- `MemoryComposition.hpp`
- `MemoryTypes.hpp`

## Documented declarations

### `TByteOperationsProvider`

**Classification:** INTERNAL PROVIDER API

Validates the complete public contract required from a ByteOperations provider.

```cpp
template<class TByteOperationsProvider>
    struct ByteOperationsProviderTraits
```

### `CopyResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by CopyBytes.

```cpp
using CopyResult = decltype(
            std::declval<const TByteOperationsProvider&>().CopyBytes(
                std::declval<void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );
```

### `MoveResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by MoveBytes.

```cpp
using MoveResult = decltype(
            std::declval<const TByteOperationsProvider&>().MoveBytes(
                std::declval<void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );
```

### `FillResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by FillBytes.

```cpp
using FillResult = decltype(
            std::declval<const TByteOperationsProvider&>().FillBytes(
                std::declval<void*>(),
                std::declval<std::uint8_t>(),
                std::declval<std::size_t>()
            )
        );
```

### `CompareResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by CompareBytes.

```cpp
using CompareResult = decltype(
            std::declval<const TByteOperationsProvider&>().CompareBytes(
                std::declval<const void*>(),
                std::declval<const void*>(),
                std::declval<std::size_t>()
            )
        );
```

