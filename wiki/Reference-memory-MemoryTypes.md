# src/memory/MemoryTypes.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35475501654b39cc2b8a9f2032a00e9d30fa3058`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35475501654b39cc2b8a9f2032a00e9d30fa3058/src/memory/MemoryTypes.hpp)

## Direct includes

- `cstddef`
- `cstdint`

## Documented declarations

### `ByteComparison`

**Classification:** PUBLIC API

Lexicographical ordering produced by raw byte comparison.

```cpp
enum class ByteComparison : std::uint8_t
```

### `MemoryAllocationResult`

**Classification:** PUBLIC API

Outcome from requesting one raw aligned allocation from a MemoryResource provider.

```cpp
enum class MemoryAllocationResult : std::uint8_t
```

### `MemoryReleaseResult`

**Classification:** PUBLIC API

Outcome from returning one raw allocation to its originating MemoryResource provider.

```cpp
enum class MemoryReleaseResult : std::uint8_t
```

### `SharedReserveInitializationResult`

**Classification:** PUBLIC API

Outcome from initializing a shared-reserve allocation algorithm over one fixed backing block.

```cpp
enum class SharedReserveInitializationResult : std::uint8_t
```

### `SharedAllocationResult`

**Classification:** PUBLIC API

Outcome from requesting one allocation from the bounded shared reserve.

```cpp
enum class SharedAllocationResult : std::uint8_t
```

### `SharedAllocationReleaseResult`

**Classification:** PUBLIC API

Outcome from returning one allocation to the bounded shared reserve.

```cpp
enum class SharedAllocationReleaseResult : std::uint8_t
```

### `SharedAllocatorTeardownResult`

**Classification:** PUBLIC API

Outcome from tearing down one shared-reserve allocator instance.

```cpp
enum class SharedAllocatorTeardownResult : std::uint8_t
```

### `MemoryTopologyState`

**Classification:** PUBLIC API

Runtime lifecycle state of one configured Memory topology.

```cpp
enum class MemoryTopologyState : std::uint8_t
```

### `MemoryTopologyInitializationResult`

**Classification:** PUBLIC API

Outcome from synchronously establishing one complete configured Memory topology.

```cpp
enum class MemoryTopologyInitializationResult : std::uint8_t
```

### `PendingAcquisitionCancellationResult`

**Classification:** PUBLIC API

Outcome from cancelling acquisition requests that are presently waiting for capacity.

```cpp
enum class PendingAcquisitionCancellationResult : std::uint8_t
```

### `MemoryTopologyTeardownResult`

**Classification:** PUBLIC API

Outcome from tearing down one initialized Memory topology.

```cpp
enum class MemoryTopologyTeardownResult : std::uint8_t
```

### `ObjectPoolAcquisitionResult`

**Classification:** PUBLIC API

Outcome from acquiring one live object from an ObjectPool.

```cpp
enum class ObjectPoolAcquisitionResult : std::uint8_t
```

### `ObjectPoolLeaseReleaseResult`

**Classification:** PUBLIC API

Outcome from explicitly releasing one ObjectPoolLease.

```cpp
enum class ObjectPoolLeaseReleaseResult : std::uint8_t
```

### `MemoryBlock`

**Classification:** PUBLIC API

Raw aligned allocation owned by the MemoryResource provider that produced it.

```cpp
struct MemoryBlock final
```

### `Address`

**Classification:** PUBLIC API · source access: `public`

First writable byte of the allocation, or null only for an empty descriptor.

```cpp
void* Address = nullptr;
```

### `Size`

**Classification:** PUBLIC API · source access: `public`

Actual usable byte count owned by this allocation.

```cpp
std::size_t Size = 0U;
```

### `Alignment`

**Classification:** PUBLIC API · source access: `public`

Alignment satisfied by Address.

```cpp
std::size_t Alignment = 0U;
```

### `SharedAllocation`

**Classification:** PUBLIC API

Opaque allocation returned by a shared-reserve allocation algorithm.

```cpp
struct SharedAllocation final
```

### `PayloadOffset`

**Classification:** PUBLIC API · source access: `public`

Byte offset of the live payload from the start of the shared reserve.

```cpp
std::size_t PayloadOffset = 0U;
```

### `MemoryTopologyInitializationFailure`

**Classification:** PUBLIC API

Transient diagnostic context populated when topology initialization fails.

```cpp
struct MemoryTopologyInitializationFailure final
```

### `Result`

**Classification:** PUBLIC API · source access: `public`

Initialization result associated with this context.

```cpp
MemoryTopologyInitializationResult Result = MemoryTopologyInitializationResult::Succeeded;
```

### `ObjectPoolIndex`

**Classification:** PUBLIC API · source access: `public`

Zero-based ObjectPoolSpec ordinal, or NoObjectPoolIndex when failure is not pool-specific.

```cpp
std::size_t ObjectPoolIndex = static_cast<std::size_t>(-1);
```

### `ResourceResult`

**Classification:** PUBLIC API · source access: `public`

Underlying raw resource result when resource allocation caused the failure.

```cpp
MemoryAllocationResult ResourceResult = MemoryAllocationResult::Succeeded;
```

### `NoObjectPoolIndex`

**Classification:** PUBLIC API · source access: `public`

Reserved ordinal used when no Object Pool is associated with the failure.

```cpp
static constexpr std::size_t NoObjectPoolIndex = static_cast<std::size_t>(-1);
```

