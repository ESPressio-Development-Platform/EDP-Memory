# src/memory/MemoryTopology.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/MemoryTopology.hpp)

## Direct includes

- `cstddef`
- `tuple`
- `ObjectPoolConfiguration.hpp`
- `detail/TopologyTraits.hpp`

## Documented declarations

### `TDefaultMemoryResourceProvider`

**Classification:** PUBLIC API

Complete compile-time declaration of one immutable Memory topology.

- **Template parameter `TDefaultMemoryResourceProvider`:** Default dedicated MemoryResource provider selected by Bootstrap.
- **Template parameter `TSharedReserve`:** SharedReserve declaration for the one v1 overflow reserve.
- **Template parameter `TObjectPoolSpecs`:** ObjectPoolSpec declarations comprising the topology.

```cpp
template<
        class TDefaultMemoryResourceProvider,
```

### `DefaultMemoryResourceProvider`

**Classification:** PUBLIC API · source access: `public`

Default dedicated MemoryResource provider.

```cpp
using DefaultMemoryResourceProvider = TDefaultMemoryResourceProvider;
```

### `SharedReserveSpec`

**Classification:** PUBLIC API · source access: `public`

One shared-reserve declaration for this topology.

```cpp
using SharedReserveSpec = TSharedReserve;
```

### `ObjectPoolSpecs`

**Classification:** PUBLIC API · source access: `public`

Ordered tuple of ObjectPoolSpec declarations.

```cpp
using ObjectPoolSpecs = std::tuple<TObjectPoolSpecs...>;
```

### `ObjectPoolCount`

**Classification:** PUBLIC API · source access: `public`

Number of Object Pools configured by this topology.

```cpp
static constexpr std::size_t ObjectPoolCount = sizeof...(TObjectPoolSpecs);
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Reports whether this topology contains an Object Pool for TObject.

```cpp
template<class TObject>
        static constexpr bool ContainsObjectPool = !std::is_void_v<
            typename Detail::FindObjectPoolSpec<TObject, TObjectPoolSpecs...>::Type
        >;
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Resolves the ObjectPoolSpec configured for TObject.

```cpp
template<class TObject>
        using ObjectPoolSpecFor = typename Detail::FindObjectPoolSpec<TObject, TObjectPoolSpecs...>::Type;
```

### `TObject`

**Classification:** PUBLIC API · source access: `public`

Returns the zero-based declaration ordinal of TObject's ObjectPoolSpec.

```cpp
template<class TObject>
        static constexpr std::size_t ObjectPoolIndex = Detail::FindObjectPoolSpec<
            TObject,
```

### `TObjectPoolSpec`

**Classification:** PUBLIC API · source access: `public`

Resolves the dedicated MemoryResource provider selected for one ObjectPoolSpec.

```cpp
template<class TObjectPoolSpec>
        using ResourceProviderFor = typename Detail::ResolveObjectPoolResource<
            TDefaultMemoryResourceProvider,
```

