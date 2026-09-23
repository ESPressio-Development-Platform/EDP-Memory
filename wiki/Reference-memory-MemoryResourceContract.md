# src/memory/MemoryResourceContract.hpp

**Primary classification:** INTERNAL PROVIDER API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/MemoryResourceContract.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `utility`
- `MemoryComposition.hpp`
- `MemoryTypes.hpp`

## Documented declarations

### `TMemoryResourceProvider`

**Classification:** INTERNAL PROVIDER API

Validates the complete public contract required from a MemoryResource provider.

```cpp
template<class TMemoryResourceProvider>
    struct MemoryResourceProviderTraits
```

### `AllocateResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by Allocate.

```cpp
using AllocateResult = decltype(
            std::declval<TMemoryResourceProvider&>().Allocate(
                std::declval<std::size_t>(),
                std::declval<std::size_t>(),
                std::declval<MemoryBlock&>()
            )
        );
```

### `ReleaseResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by Release.

```cpp
using ReleaseResult = decltype(
            std::declval<TMemoryResourceProvider&>().Release(
                std::declval<const MemoryBlock&>()
            )
        );
```

