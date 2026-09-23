# src/memory/detail/ObjectPoolState.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/detail/ObjectPoolState.hpp)

## Direct includes

- `array`
- `cstddef`
- `cstdint`
- `../MemoryResourceContract.hpp`
- `../ObjectPoolConfiguration.hpp`

## Documented declarations

### `ObjectPoolCapacityClaimResult`

**Classification:** PRIVATE IMPLEMENTATION

Outcome from attempting to claim one bounded Object Pool capacity slot.

```cpp
enum class ObjectPoolCapacityClaimResult : std::uint8_t
```

### `ObjectPoolCapacityReturnResult`

**Classification:** PRIVATE IMPLEMENTATION

Outcome from returning a previously claimed but unconstructed Object Pool capacity slot.

```cpp
enum class ObjectPoolCapacityReturnResult : std::uint8_t
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION

Empty dedicated state used when an Object Pool reserves no dedicated instances.

```cpp
template<class TObject, std::size_t TCount>
    class DedicatedObjectPoolState;
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION

Zero-capacity dedicated state requiring no backing descriptor or occupancy bytes.

```cpp
template<class TObject>
    class DedicatedObjectPoolState<TObject, 0U>
```

### `HasLiveDedicatedObjects`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports that this pool has no dedicated live objects.

```cpp
bool HasLiveDedicatedObjects() const noexcept
```

### `TryClaimDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports that no dedicated slot can be claimed.

```cpp
ObjectPoolCapacityClaimResult TryClaimDedicated(
                std::size_t&
            ) noexcept
```

### `ReleaseDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Ignores release because no dedicated slot exists.

```cpp
void ReleaseDedicated(
                std::size_t
            ) noexcept
```

### `DedicatedAddress`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns null because no dedicated slot exists.

```cpp
TObject* DedicatedAddress(
                std::size_t
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Performs no allocation for a zero-capacity dedicated pool.

```cpp
template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider&
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Performs no release for a zero-capacity dedicated pool.

```cpp
template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider&
            ) noexcept
```

### `TObject`

**Classification:** PRIVATE IMPLEMENTATION

Fixed dedicated storage and occupancy metadata for a non-zero Object Pool capacity.

```cpp
template<class TObject, std::size_t TCount>
    class DedicatedObjectPoolState
```

### `MemoryBlock _backing{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Raw backing allocation containing exactly TCount object slots.

```cpp
MemoryBlock _backing{};
```

### `uint8_t`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

One occupancy bit per dedicated object slot.

```cpp
std::array<std::uint8_t, (TCount + 7U) / 8U> _occupancy{};
```

### `IsOccupied`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether one dedicated slot is occupied.

```cpp
bool IsOccupied(
                std::size_t slotIndex
            ) const noexcept
```

### `SetOccupied`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Sets or clears one dedicated slot occupancy bit.

```cpp
void SetOccupied(
                std::size_t slotIndex,
                bool occupied
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Acquires exactly one contiguous backing allocation for every dedicated slot.

```cpp
template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider& resource
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Releases the complete dedicated backing allocation when no live object remains.

```cpp
template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider& resource
            ) noexcept
```

### `TryClaimDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Claims the lowest-index vacant dedicated slot.

```cpp
ObjectPoolCapacityClaimResult TryClaimDedicated(
                std::size_t& slotIndex
            ) noexcept
```

### `ReleaseDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Returns one previously claimed dedicated slot to the vacant set.

```cpp
void ReleaseDedicated(
                std::size_t slotIndex
            ) noexcept
```

### `DedicatedAddress`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Resolves the object address for one dedicated slot.

```cpp
TObject* DedicatedAddress(
                std::size_t slotIndex
            ) noexcept
```

### `HasLiveDedicatedObjects`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether any dedicated slot is currently claimed.

```cpp
bool HasLiveDedicatedObjects() const noexcept
```

### `TSharedPolicy`

**Classification:** PRIVATE IMPLEMENTATION

Shared-overflow quota state selected from one ObjectPoolSpec policy.

```cpp
template<
        class TSharedPolicy,
```

### `TSharedPolicy`

**Classification:** PRIVATE IMPLEMENTATION

Empty state used when shared overflow is disabled or carries no per-Type quota.

```cpp
template<class TSharedPolicy>
    class SharedObjectPoolQuotaState<TSharedPolicy, false>
```

### `CanClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether this policy permits attempting a shared allocation.

```cpp
bool CanClaimShared() const noexcept
```

### `ClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Records no quota state because this policy has no finite per-Type quota.

```cpp
void ClaimShared() noexcept
```

### `ReleaseShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Releases no quota state because this policy has no finite per-Type quota.

```cpp
void ReleaseShared() noexcept
```

### `TSharedPolicy`

**Classification:** PRIVATE IMPLEMENTATION

One-byte quota state used only when a finite shared instance limit is configured.

```cpp
template<class TSharedPolicy>
    class SharedObjectPoolQuotaState<TSharedPolicy, true>
```

### `_count`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Current simultaneous shared-reserve instance count for this Object Pool.

```cpp
std::uint8_t _count = 0U;
```

### `CanClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Reports whether another shared instance is permitted by the configured quota.

```cpp
bool CanClaimShared() const noexcept
```

### `ClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Records one newly claimed shared instance.

```cpp
void ClaimShared() noexcept
```

### `ReleaseShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Records return of one shared instance.

```cpp
void ReleaseShared() noexcept
```

