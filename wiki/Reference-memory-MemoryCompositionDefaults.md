# src/memory/MemoryCompositionDefaults.hpp

**Primary classification:** INTERNAL COMPOSITION API

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/MemoryCompositionDefaults.hpp)

## Direct includes

- `cstddef`
- `CoalescingFirstFitProvider.hpp`
- `MemoryComposition.hpp`

## Documented declarations

### `SharedAllocatorProviderCountV`

**Classification:** INTERNAL COMPOSITION API

Counts providers that explicitly supply the shared-reserve allocation capability.

```cpp
template<class... TProviders>
    inline constexpr std::size_t SharedAllocatorProviderCountV = (
        std::size_t{0U} + ... + (
            TProviders::CompositionOffers::template Contains<SharedReserveAllocationAlgorithm>
                ? std::size_t{1U}
                : std::size_t{0U}
        )
    );
```

### `THasExplicitProvider`

**Classification:** INTERNAL COMPOSITION API

Selects the explicit shared-reserve algorithm or appends CoalescingFirstFitProvider by default.

```cpp
template<
        bool THasExplicitProvider,
```

### `DefaultMemoryCompositionSelector`

**Classification:** INTERNAL COMPOSITION API

Builds a Memory Composition retaining the explicitly selected allocator provider.

```cpp
template<class... TProviders>
    struct DefaultMemoryCompositionSelector<true, TProviders...>
```

### `Type`

**Classification:** INTERNAL COMPOSITION API · source access: `public`

Complete Memory Composition using the explicitly supplied allocation algorithm.

```cpp
using Type = Framework::Composition<
            Domain,
```

### `DefaultMemoryCompositionSelector`

**Classification:** INTERNAL COMPOSITION API

Builds a Memory Composition using the default coalescing-first-fit allocation algorithm.

```cpp
template<class... TProviders>
    struct DefaultMemoryCompositionSelector<false, TProviders...>
```

### `Type`

**Classification:** INTERNAL COMPOSITION API · source access: `public`

Complete Memory Composition with CoalescingFirstFitProvider appended at compile time.

```cpp
using Type = Framework::Composition<
            Domain,
```

### `MemoryComposition`

**Classification:** INTERNAL COMPOSITION API

Builds one compile-time Memory Composition and injects CoalescingFirstFitProvider when no
shared-reserve allocation algorithm is explicitly supplied by Bootstrap.

- **Template parameter `TProviders`:** Explicit Memory capability providers selected by application Bootstrap.

```cpp
template<class... TProviders>
    using MemoryComposition = typename Detail::DefaultMemoryCompositionSelector<
        Detail::SharedAllocatorProviderCountV<TProviders...> != 0U,
```

