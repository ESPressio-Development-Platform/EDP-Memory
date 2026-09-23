# src/memory/ObjectPoolLease.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/ObjectPoolLease.hpp)

## Direct includes

- `cstddef`
- `utility`
- `MemoryTypes.hpp`
- `detail/ObjectPoolToken.hpp`

## Documented declarations

### `TObject`

**Classification:** PUBLIC API

Move-only RAII ownership of one live object acquired from an Object Pool.

- **Template parameter `TObject`:** Pooled object Type.
- **Template parameter `TObjectPool`:** Concrete ObjectPool Type that owns the storage represented by this lease.

```cpp
template<class TObject, class TObjectPool>
    class ObjectPoolLease final
```

### `_pool`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Non-owning pointer to the Object Pool responsible for this lease.

```cpp
TObjectPool* _pool = nullptr;
```

### `Empty`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Compact dedicated/shared allocation token owned by this lease.

```cpp
std::size_t _token = Detail::ObjectPoolToken::Empty();
```

### `Adopt`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Adopts a newly constructed pooled object.

```cpp
void Adopt(
                TObjectPool& pool,
                std::size_t token
            ) noexcept
```

### `ObjectPoolLease`

**Classification:** PUBLIC API · source access: `public`

Creates an empty lease owning no object.

```cpp
ObjectPoolLease() = default;
```

### `ObjectPoolLease`

**Classification:** PUBLIC API · source access: `public`

Prevents duplicate ownership of one pooled object.

```cpp
ObjectPoolLease(const ObjectPoolLease&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents duplicate ownership by copy assignment.

```cpp
ObjectPoolLease& operator =(const ObjectPoolLease&) = delete;
```

### `ObjectPoolLease`

**Classification:** PUBLIC API · source access: `public`

Transfers ownership from another lease and leaves the source empty.

```cpp
ObjectPoolLease(
                ObjectPoolLease&& source
            ) noexcept :
                _pool(source._pool),
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Releases current ownership before transferring ownership from the source lease.

```cpp
ObjectPoolLease& operator =(
                ObjectPoolLease&& source
            ) noexcept
```

### `ObjectPoolLease`

**Classification:** PUBLIC API · source access: `public`

Returns owned capacity automatically when this lease leaves scope.

```cpp
~ObjectPoolLease() noexcept
```

### `IsEmpty`

**Classification:** PUBLIC API · source access: `public`

Reports whether this lease owns no pooled object.

```cpp
bool IsEmpty() const noexcept
```

### `Get`

**Classification:** PUBLIC API · source access: `public`

Returns a non-owning pointer valid only while this lease owns a live object.

```cpp
TObject* Get() noexcept
```

### `Get`

**Classification:** PUBLIC API · source access: `public`

Returns a non-owning pointer valid only while this lease owns a live object.

```cpp
const TObject* Get() const noexcept
```

### `TObject& operator *() noexcept`

**Classification:** PUBLIC API · source access: `public`

Dereferences the live object owned by this lease.

```cpp
TObject& operator *() noexcept
```

### `const TObject& operator *() const noexcept`

**Classification:** PUBLIC API · source access: `public`

Dereferences the live object owned by this lease.

```cpp
const TObject& operator *() const noexcept
```

### `TObject* operator ->() noexcept`

**Classification:** PUBLIC API · source access: `public`

Provides member access to the live object owned by this lease.

```cpp
TObject* operator ->() noexcept
```

### `const TObject* operator ->() const noexcept`

**Classification:** PUBLIC API · source access: `public`

Provides member access to the live object owned by this lease.

```cpp
const TObject* operator ->() const noexcept
```

### `Release`

**Classification:** PUBLIC API · source access: `public`

Destroys the owned object when necessary and returns its capacity to the Object Pool.

```cpp
ObjectPoolLeaseReleaseResult Release() noexcept
```

