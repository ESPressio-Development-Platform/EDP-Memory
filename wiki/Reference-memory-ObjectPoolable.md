# src/memory/ObjectPoolable.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/ObjectPoolable.hpp)

## Direct includes

- `type_traits`
- `utility`

## Documented declarations

### `TDerived`

**Classification:** PUBLIC API

Optional zero-state CRTP convenience for Types commonly constructed through ObjectPool.

This helper performs no hidden lookup and stores no pool binding. The caller must still
supply the exact ObjectPool and wait policy explicitly at each acquisition site.

- **Template parameter `TDerived`:** Concrete pooled Type exposing this convenience surface.

```cpp
template<class TDerived>
    class ObjectPoolable
```

### `ObjectPoolable`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Allows only derived Types to construct this zero-state convenience base.

```cpp
ObjectPoolable() = default;
```

### `ObjectPoolable`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Allows derived Types to use ordinary copy construction semantics for this empty base.

```cpp
ObjectPoolable(const ObjectPoolable&) = default;
```

### `ObjectPoolable`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Allows derived Types to use ordinary move construction semantics for this empty base.

```cpp
ObjectPoolable(ObjectPoolable&&) = default;
```

### `operator`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Allows derived Types to use ordinary copy assignment semantics for this empty base.

```cpp
ObjectPoolable& operator =(const ObjectPoolable&) = default;
```

### `operator`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Allows derived Types to use ordinary move assignment semantics for this empty base.

```cpp
ObjectPoolable& operator =(ObjectPoolable&&) = default;
```

### `ObjectPoolable`

**Classification:** INTERNAL / PROTECTED API · source access: `protected`

Provides protected destruction because this base is not a polymorphic ownership boundary.

```cpp
~ObjectPoolable() = default;
```

### `TObjectPool`

**Classification:** PUBLIC API · source access: `public`

Acquires one TDerived from the supplied ObjectPool using the explicit caller wait policy.

- **Template parameter `TObjectPool`:** Concrete ObjectPool managing TDerived.
- **Template parameter `TWaitTimeout`:** Platform wait-policy Type accepted by TObjectPool.
- **Template parameter `TArguments`:** Constructor argument Types forwarded to TDerived.
- **Parameter `pool`:** Explicit pool selected by the caller or Bootstrap wiring.
- **Parameter `lease`:** Empty output lease that receives ownership only on success.
- **Parameter `timeout`:** Explicit caller-selected wait policy.
- **Parameter `arguments`:** Constructor arguments forwarded to TDerived.

```cpp
template<
                class TObjectPool,
```

