# src/memory/detail/TopologyTraits.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/detail/TopologyTraits.hpp)

## Direct includes

- `cstddef`
- `tuple`
- `type_traits`
- `../ObjectPoolConfiguration.hpp`

## Documented declarations

### `TNeedle`

**Classification:** PRIVATE IMPLEMENTATION

Indicates whether one Type appears in a Type pack.

```cpp
template<class TNeedle, class... THaystack>
    inline constexpr bool ContainsTypeV = (std::is_same_v<TNeedle, THaystack> || ...);
```

### `UniqueObjectPoolTypes`

**Classification:** PRIVATE IMPLEMENTATION

Validates that every ObjectPoolSpec owns a distinct object Type.

```cpp
template<class... TObjectPoolSpecs>
    struct UniqueObjectPoolTypes;
```

### `UniqueObjectPoolTypes`

**Classification:** PRIVATE IMPLEMENTATION

Empty pool-specification packs are unique.

```cpp
template<>
    struct UniqueObjectPoolTypes<> : std::true_type {};
```

### `TFirstSpec`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Checks one pool Type against the remaining pool Types recursively.

```cpp
template<class TFirstSpec, class... TRestSpecs>
    struct UniqueObjectPoolTypes<TFirstSpec, TRestSpecs...> : std::bool_constant<
        (!std::is_same_v<typename TFirstSpec::Object, typename TRestSpecs::Object> && ...) &&
        UniqueObjectPoolTypes<TRestSpecs...>::value
    > {};
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves an ObjectPoolSpec by its managed object Type.

```cpp
template<class TObject, class... TObjectPoolSpecs>
    struct FindObjectPoolSpec;
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Represents an unsuccessful ObjectPoolSpec lookup.

```cpp
template<class TObject>
    struct FindObjectPoolSpec<TObject>
```

### `Type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Missing specification marker.

```cpp
using Type = void;
```

### `Index`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Sentinel index returned when no matching ObjectPoolSpec exists.

```cpp
static constexpr std::size_t Index = static_cast<std::size_t>(-1);
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves the first ObjectPoolSpec managing TObject.

```cpp
template<class TObject, class TFirstSpec, class... TRestSpecs>
    struct FindObjectPoolSpec<TObject, TFirstSpec, TRestSpecs...>
```

### `Remaining`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Recursive lookup result for the remaining specifications.

```cpp
using Remaining = FindObjectPoolSpec<TObject, TRestSpecs...>;
```

### `Type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Matching ObjectPoolSpec, or the recursive result when the first specification differs.

```cpp
using Type = std::conditional_t<
                std::is_same_v<TObject, typename TFirstSpec::Object>,
```

### `Index`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Zero-based topology ordinal of the matching ObjectPoolSpec.

```cpp
static constexpr std::size_t Index = std::is_same_v<TObject, typename TFirstSpec::Object>
                ? 0U
                : (
                    Remaining::Index == static_cast<std::size_t>(-1)
                        ? static_cast<std::size_t>(-1)
                        : Remaining::Index + 1U
                );
```

### `TDefaultMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves the dedicated MemoryResource provider selected by one ObjectPoolSpec.

```cpp
template<class TDefaultMemoryResourceProvider, class TResourceSelection>
    struct ResolveObjectPoolResource;
```

### `TDefaultMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves the topology default MemoryResource provider.

```cpp
template<class TDefaultMemoryResourceProvider>
    struct ResolveObjectPoolResource<TDefaultMemoryResourceProvider, UseDefaultMemoryResource>
```

### `Type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Concrete MemoryResource provider selected for the pool.

```cpp
using Type = TDefaultMemoryResourceProvider;
```

### `TDefaultMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves one explicit Object Pool MemoryResource override.

```cpp
template<class TDefaultMemoryResourceProvider, class TMemoryResourceProvider>
    struct ResolveObjectPoolResource<
        TDefaultMemoryResourceProvider,
```

### `Type`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Concrete MemoryResource provider selected for the pool.

```cpp
using Type = TMemoryResourceProvider;
```

### `AnySharedOverflowEnabledV`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Indicates whether any ObjectPoolSpec enables shared overflow.

```cpp
template<class... TObjectPoolSpecs>
    inline constexpr bool AnySharedOverflowEnabledV = (false || ... || TObjectPoolSpecs::Shared::IsEnabled);
```

