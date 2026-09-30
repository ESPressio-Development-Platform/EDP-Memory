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

## Event V1 indexed dedicated-pool additions

### `DedicatedObjectPoolAcquisitionResult`

**Classification:** PUBLIC API

Strong operational result for non-waiting dedicated-index acquisition.

- `Succeeded = 0` — one dedicated slot was claimed, the object was constructed, and the output index now owns that slot.
- `CapacityUnavailable = 1` — no dedicated slot was vacant; shared capacity is intentionally not considered.
- `NotInitialized = 2` — the owning Memory runtime has not established its topology.
- `TopologyUnavailable = 3` — the topology is not in its frozen operational state or acquisitions have been cancelled.
- `OutputIndexOccupied = 4` — the caller supplied an already-valid ownership index; it is left unchanged.
- `ProviderFailure = 5` — the Memory synchronization/provider layer could not preserve normal operation.

### `DedicatedObjectPoolResetResult`

**Classification:** PUBLIC API

Strong operational result for in-place reconstruction of one live indexed dedicated object.

- `Succeeded = 0` — the existing object was destroyed and reconstructed at the same address while its capacity claim/index remained live.
- `InvalidIndex = 1` — the supplied index is invalid.
- `SlotNotOwned = 2` — the numeric index is in range but no live dedicated claim owns that slot.
- `NotInitialized = 3` — the Memory runtime is uninitialized.
- `TopologyUnavailable = 4` — the topology is not in normal frozen/acquisition-enabled operation.
- `ProviderFailure = 5` — synchronization/provider failure prevented validation from reaching the reconstruction boundary.

### `DedicatedObjectPoolReleaseResult`

**Classification:** PUBLIC API

Strong operational result for indexed dedicated-object destruction/capacity return.

- `Released = 0` — object lifetime ended, the dedicated slot returned to capacity, and the caller index was invalidated.
- `InvalidIndex = 1` — the supplied index is already invalid and therefore owns no slot.
- `SlotNotOwned = 2` — the numeric index is in-range but the corresponding slot is not currently claimed.
- `NotInitialized = 3` — the Memory runtime is uninitialized.
- `TopologyUnavailable = 4` — release was attempted while the topology is outside its normal frozen operational state.
- `ProviderFailure = 5` — synchronization/provider failure prevented a cleanly reportable normal release transition.

