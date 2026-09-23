# src/memory/SharedReserveAllocationContract.hpp

**Primary classification:** INTERNAL PROVIDER API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/SharedReserveAllocationContract.hpp)

## Direct includes

- `cstddef`
- `type_traits`
- `utility`
- `MemoryComposition.hpp`
- `MemoryTypes.hpp`

## Documented declarations

### `TSharedReserveAllocationProvider`

**Classification:** INTERNAL PROVIDER API

Validates the complete public contract required from a SharedReserveAllocationAlgorithm provider.

```cpp
template<class TSharedReserveAllocationProvider>
    struct SharedReserveAllocationProviderTraits
```

### `InitializeResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by Initialize.

```cpp
using InitializeResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Initialize(
                std::declval<const MemoryBlock&>()
            )
        );
```

### `AllocateResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by Allocate.

```cpp
using AllocateResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Allocate(
                std::declval<std::size_t>(),
                std::declval<std::size_t>(),
                std::declval<SharedAllocation&>()
            )
        );
```

### `ReleaseResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by Release.

```cpp
using ReleaseResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().Release(
                std::declval<const SharedAllocation&>()
            )
        );
```

### `TearDownResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by TearDown.

```cpp
using TearDownResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().TearDown()
        );
```

### `HasLiveAllocationsResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by the live-allocation predicate.

```cpp
using HasLiveAllocationsResult = decltype(
            std::declval<const TSharedReserveAllocationProvider&>().HasLiveAllocations()
        );
```

### `AddressResult`

**Classification:** INTERNAL PROVIDER API · source access: `public`

Return type produced by resolving a shared allocation address.

```cpp
using AddressResult = decltype(
            std::declval<TSharedReserveAllocationProvider&>().AddressOf(
                std::declval<const SharedAllocation&>()
            )
        );
```

