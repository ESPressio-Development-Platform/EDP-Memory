# src/memory/ObjectPool.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `68ba27e5cfc30fdbc43237c4c08557e79f25c5d3`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/68ba27e5cfc30fdbc43237c4c08557e79f25c5d3/src/memory/ObjectPool.hpp)

## Direct includes

- `ESPressio_Platform.hpp`
- `cstddef`
- `type_traits`
- `utility`
- `MemoryTypes.hpp`
- `ObjectPoolLease.hpp`
- `detail/ObjectPoolState.hpp`
- `detail/ObjectPoolToken.hpp`

## Documented declarations

### `TObject`

**Classification:** PUBLIC API

Consumer-facing pool for one object Type within a concrete MemoryRuntime.

The pool owns only bounded backing/occupancy metadata. Application code receives a
non-owning reference to this object from MemoryRuntime::ObjectPoolFor<TObject>().

- **Template parameter `TObject`:** Object Type managed by this pool.
- **Template parameter `TObjectPoolSpec`:** Compile-time specification governing this pool.
- **Template parameter `TMemoryRuntime`:** Concrete MemoryRuntime that coordinates this pool.

```cpp
template<class TObject, class TObjectPoolSpec, class TMemoryRuntime>
    class ObjectPool final : private Detail::DedicatedObjectPoolState<
        TObject,
```

### `DedicatedState`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Dedicated slot state for this Object Pool.

```cpp
using DedicatedState = Detail::DedicatedObjectPoolState<
                TObject,
```

### `SharedQuotaState`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Shared quota state for this Object Pool.

```cpp
using SharedQuotaState = Detail::SharedObjectPoolQuotaState<
                typename TObjectPoolSpec::Shared
            >;
```

### `_runtime`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Non-owning pointer to the lifetime-stable MemoryRuntime coordinating this pool.

```cpp
TMemoryRuntime* _runtime = nullptr;
```

### `BindRuntime`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Binds this facade to its lifetime-stable owning MemoryRuntime during runtime construction.

```cpp
void BindRuntime(
                TMemoryRuntime& runtime
            ) noexcept
```

### `TMemoryRuntime`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Allows MemoryRuntime to initialize and coordinate private pool state.

```cpp
friend TMemoryRuntime;
```

### `ObjectPool`

**Classification:** PUBLIC API · source access: `public`

Creates an unbound Object Pool facade for internal tuple construction.

```cpp
ObjectPool() noexcept = default;
```

### `LeaseType`

**Classification:** PUBLIC API · source access: `public`

Move-only ownership Type returned by successful acquisition from this pool.

```cpp
using LeaseType = ObjectPoolLease<TObject, ObjectPool>;
```

### `ObjectType`

**Classification:** PUBLIC API · source access: `public`

Object Type managed by this pool.

```cpp
using ObjectType = TObject;
```

### `Spec`

**Classification:** PUBLIC API · source access: `public`

Compile-time ObjectPoolSpec selected for this object Type.

```cpp
using Spec = TObjectPoolSpec;
```

### `ObjectPool`

**Classification:** PUBLIC API · source access: `public`

Prevents copying pool runtime state.

```cpp
ObjectPool(const ObjectPool&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents copy assignment of pool runtime state.

```cpp
ObjectPool& operator =(const ObjectPool&) = delete;
```

### `ObjectPool`

**Classification:** PUBLIC API · source access: `public`

Prevents moving pool runtime state after references may have escaped to consumers.

```cpp
ObjectPool(ObjectPool&&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents move assignment of pool runtime state.

```cpp
ObjectPool& operator =(ObjectPool&&) = delete;
```

### `Acquire`

**Classification:** PUBLIC API · source access: `public`

Acquires and constructs one object according to the explicitly supplied wait policy.

- **Template parameter `TArguments`:** Constructor argument Types forwarded to TObject.
- **Parameter `lease`:** Empty output lease that receives ownership only on success.
- **Parameter `timeout`:** Explicit caller-selected wait policy.
- **Parameter `arguments`:** Constructor arguments forwarded after capacity is reserved.

```cpp
template<class... TArguments>
            ObjectPoolAcquisitionResult Acquire(
                LeaseType& lease,
                ESPressio::Platform::Synchronization::WaitTimeout timeout,
                TArguments&&... arguments
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Initializes this pool's exact dedicated backing allocation.

```cpp
template<class TMemoryResourceProvider>
            MemoryAllocationResult InitializeDedicated(
                TMemoryResourceProvider& resource
            ) noexcept
```

### `TMemoryResourceProvider`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Releases this pool's complete dedicated backing allocation.

```cpp
template<class TMemoryResourceProvider>
            MemoryReleaseResult ReleaseDedicatedBacking(
                TMemoryResourceProvider& resource
            ) noexcept
```

### `TryClaimDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Attempts to claim one dedicated slot and returns its encoded ownership token.

```cpp
Detail::ObjectPoolCapacityClaimResult TryClaimDedicated(
                std::size_t& token
            ) noexcept
```

### `CanClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether a shared allocation may be attempted under this pool's policy/quota.

```cpp
bool CanClaimShared() const noexcept
```

### `ClaimShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Records one successful shared allocation against this pool's finite quota when configured.

```cpp
void ClaimShared() noexcept
```

### `ReleaseShared`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Records release of one shared allocation against this pool's finite quota when configured.

```cpp
void ReleaseShared() noexcept
```

### `ReleaseDedicated`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns one claimed dedicated slot to the vacant set.

```cpp
void ReleaseDedicated(
                std::size_t token
            ) noexcept
```

### `AdoptLease`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Publishes one newly constructed object into the caller's empty lease.

```cpp
void AdoptLease(
                LeaseType& lease,
                std::size_t token
            ) noexcept
```

### `ResolveToken`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves one live object from the compact lease token.

```cpp
TObject* ResolveToken(
                std::size_t token
            ) noexcept
```

### `ResolveToken`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves one live object from the compact lease token for const access.

```cpp
const TObject* ResolveToken(
                std::size_t token
            ) const noexcept
```

### `ReleaseToken`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Destroys and releases the object represented by one lease token.

```cpp
ObjectPoolLeaseReleaseResult ReleaseToken(
                std::size_t& token
            ) noexcept
```

### `DedicatedAddress`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Resolves the address associated with one dedicated token.

```cpp
TObject* DedicatedAddress(
                std::size_t token
            ) noexcept
```

### `HasLiveDedicatedObjects`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether any dedicated slot remains claimed.

```cpp
bool HasLiveDedicatedObjects() const noexcept
```

