# src/memory/ObjectLifetime.hpp

**Source classification:** PUBLIC API

`ObjectLifetime` centralizes deterministic C++ object-lifetime transitions for storage already owned by a caller. It allocates no storage and requires each selected construction, move-construction, and destruction operation to be non-throwing.

## `ObjectLifetime`

**Classification:** PUBLIC API

Stateless utility Type. It retains no members, owns no resource, requires no synchronization, and creates no independent allocation policy.

### `Construct<TObject,TArguments...>(void* storage,TArguments&&... arguments)`

Constructs `TObject` in suitably aligned caller-owned storage using `std::construct_at`. `TObject` and the supplied constructor argument pack determine the selected constructor; a compile-time assertion requires that constructor to be `noexcept`. The returned pointer addresses the newly live object. Failure is a compile-time contract violation rather than a runtime result.

### `MoveConstruct<TObject>(void* destination,TObject& source)`

Move-constructs `TObject` into caller-owned destination storage. `TObject` must be nothrow move-constructible. The source remains live in its moved-from state and its existing owner remains responsible for destroying it exactly once.

### `Destroy<TObject>(TObject& object)`

Ends the lifetime of one live `TObject` using `std::destroy_at` without releasing or otherwise mutating its backing storage. `TObject` must be nothrow destructible.

## Resource, concurrency and ISR semantics

The Type contributes zero retained runtime state. Each operation mutates only caller-supplied object/storage state and supplies no synchronization; callers must already own or appropriately synchronize that state. No ISR-safety guarantee is made.
