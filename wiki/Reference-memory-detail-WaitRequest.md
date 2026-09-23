# src/memory/detail/WaitRequest.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/detail/WaitRequest.hpp)

## Direct includes

- `cstddef`
- `cstdint`

## Documented declarations

### `WaitRequestState`

**Classification:** PRIVATE IMPLEMENTATION

Internal lifecycle of one stack-resident pending Object Pool acquisition.

```cpp
enum class WaitRequestState : std::uint8_t
```

### `WaitRequestServiceResult`

**Classification:** PRIVATE IMPLEMENTATION

Outcome from servicing pending wait requests after capacity becomes available.

```cpp
enum class WaitRequestServiceResult : std::uint8_t
```

### `TSignalProvider`

**Classification:** PRIVATE IMPLEMENTATION

Intrusive stack-resident wait request carrying one targeted Platform Signal instance.

- **Template parameter `TSignalProvider`:** Concrete Platform Signal provider selected by Bootstrap.

```cpp
template<class TSignalProvider>
    struct WaitRequest final
```

### `Previous`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Previous request in arrival order.

```cpp
WaitRequest* Previous = nullptr;
```

### `Next`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Next request in arrival order.

```cpp
WaitRequest* Next = nullptr;
```

### `ObjectPoolIndex`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Zero-based ObjectPoolSpec ordinal requested by this waiter.

```cpp
std::size_t ObjectPoolIndex = 0U;
```

### `Token`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Allocation token reserved for this waiter before notification.

```cpp
std::size_t Token = 0U;
```

### `State`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Current request lifecycle state.

```cpp
WaitRequestState State = WaitRequestState::Waiting;
```

### `TSignalProvider Signal{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Signal instance used only for the lifetime of this blocked acquisition call.

```cpp
TSignalProvider Signal{};
```

