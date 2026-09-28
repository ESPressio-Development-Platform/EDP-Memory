# src/memory/OwnershipTransfer.hpp

**Source classification:** PUBLIC API

`OwnershipTransfer` gives EDP-domain code an explicit Memory-owned vocabulary for semantic typed ownership transfer without requiring each consuming domain to reach directly for the Standard Library move primitive.

## `OwnershipTransfer`

**Classification:** PUBLIC API

Stateless utility Type. It retains no state, allocates no storage, establishes no new object lifetime, and provides no synchronization.

### `Move<TObject>(TObject&& object)`

Returns the supplied object expression as an rvalue reference to its underlying non-reference Type. The operation is `constexpr` and `noexcept` and performs no construction or destruction by itself.

The caller must still use an appropriate lifetime operation—such as `ObjectLifetime::MoveConstruct` or a consuming API whose contract explicitly transfers the value—to establish destination ownership. The source object's valid moved-from lifetime remains governed by the consuming operation.

## Resource, concurrency and ISR semantics

`OwnershipTransfer` has zero retained memory cost and no provider dependency. It merely changes value category; it does not make an otherwise unsafe caller context safe and carries no ISR-safety guarantee.
