# Typed Object Lifetime Operations

`EDP-Memory` owns the abstraction boundary for operations that begin, transfer, and end the lifetime of typed C++ objects in caller-supplied storage.

## Scope

`ObjectLifetime` does not allocate, reserve, release, or own storage. The caller must supply storage that is large enough and correctly aligned for the requested object Type and must ensure the storage remains valid for the complete object lifetime.

The facility centralizes three deterministic operations:

- `Construct<T>(storage, arguments...)` begins one `T` lifetime in supplied storage;
- `MoveConstruct<T>(destination, source)` begins one `T` lifetime in supplied destination storage by move construction from a live source object;
- `Destroy(object)` ends one live object's lifetime without releasing its storage.

`MoveConstruct` does not destroy the source. The source remains a live moved-from object and its current owner must destroy it exactly once.

## Deterministic contract

Construction is accepted only when the selected constructor is `noexcept`. Move construction requires a nothrow move constructor. Destruction requires a nothrow destructor. These requirements are compile-time contracts and avoid exception rollback machinery in consumers.

## Ownership boundary

Higher-level EDP domains that require typed lifetime transitions in supplied storage consume this abstraction rather than directly invoking placement construction, `std::move` for ownership-transfer construction, or explicit destructor calls. This keeps the C++ object-lifetime mechanism resident in EDP-Memory while leaving semantic ownership decisions with the consuming domain.

Raw `ByteOperations::Move` remains a byte-range operation and must not be used as a substitute for moving non-trivial typed objects.

## Command prerequisite

The Command architecture requires Request and Response objects to occupy fixed invocation-record storage while retaining correct C++ lifetime semantics. `ObjectLifetime` supplies that lower-domain mechanism without requiring Command to allocate storage dynamically or own C++ lifetime primitives directly.
