# src/memory/CoalescingFirstFitProvider.hpp

**Primary classification:** PUBLIC API

**Source baseline:** `35d9d50fb4f0233f79a628a1457bd7345dbf2ba4`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Memory/blob/35d9d50fb4f0233f79a628a1457bd7345dbf2ba4/src/memory/CoalescingFirstFitProvider.hpp)

## Direct includes

- `cstddef`
- `cstdint`
- `new`
- `MemoryComposition.hpp`
- `MemoryTypes.hpp`

## Documented declarations

### `CoalescingFirstFitProvider`

**Classification:** PUBLIC API

Deterministic first-fit allocator for one fixed shared reserve.

Metadata is stored in-band inside the reserve. Allocation always searches from the beginning,
live payloads are never moved, and release immediately coalesces adjacent free spans.

```cpp
class CoalescingFirstFitProvider final : public Framework::Provider<
        Domain,
```

### `BlockHeader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

In-band header describing one contiguous span of the shared reserve.

```cpp
struct BlockHeader final
```

### `SpanBytes`

**Classification:** PUBLIC API · source access: `public`

Total bytes occupied by this span, including this header.

```cpp
std::size_t SpanBytes = 0U;
```

### `PayloadOffset`

**Classification:** PUBLIC API · source access: `public`

Payload offset from this header, or zero when the span is free.

```cpp
std::size_t PayloadOffset = 0U;
```

### `_base`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

First byte of the currently initialized reserve.

```cpp
std::byte* _base = nullptr;
```

### `_size`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Header-aligned usable byte count in the currently initialized reserve.

```cpp
std::size_t _size = 0U;
```

### `IsPowerOfTwo`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reports whether a non-zero integer is a power of two.

```cpp
static constexpr bool IsPowerOfTwo(
                std::size_t value
            ) noexcept
```

### `AlignUp`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Rounds a value upward to the requested power-of-two alignment.

```cpp
static constexpr std::size_t AlignUp(
                std::size_t value,
                std::size_t alignment
            ) noexcept
```

### `FirstHeader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the first block header in the reserve.

```cpp
BlockHeader* FirstHeader() noexcept
```

### `FirstHeader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the first block header in the reserve for const inspection.

```cpp
const BlockHeader* FirstHeader() const noexcept
```

### `HeaderOffset`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the byte offset of a header from the reserve start.

```cpp
std::size_t HeaderOffset(
                const BlockHeader& header
            ) const noexcept
```

### `NextHeader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the next header, or null when the supplied block reaches the reserve end.

```cpp
BlockHeader* NextHeader(
                BlockHeader& header
            ) noexcept
```

### `NextHeader`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Returns the next header for const inspection.

```cpp
const BlockHeader* NextHeader(
                const BlockHeader& header
            ) const noexcept
```

### `CoalescingFirstFitProvider`

**Classification:** PUBLIC API · source access: `public`

Creates an uninitialized shared-reserve allocator.

```cpp
CoalescingFirstFitProvider() = default;
```

### `CoalescingFirstFitProvider`

**Classification:** PUBLIC API · source access: `public`

Prevents copying allocator runtime state.

```cpp
CoalescingFirstFitProvider(const CoalescingFirstFitProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents copy assignment of allocator runtime state.

```cpp
CoalescingFirstFitProvider& operator =(const CoalescingFirstFitProvider&) = delete;
```

### `CoalescingFirstFitProvider`

**Classification:** PUBLIC API · source access: `public`

Prevents moving allocator runtime state.

```cpp
CoalescingFirstFitProvider(CoalescingFirstFitProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC API · source access: `public`

Prevents move assignment of allocator runtime state.

```cpp
CoalescingFirstFitProvider& operator =(CoalescingFirstFitProvider&&) = delete;
```

### `Initialize`

**Classification:** PUBLIC API · source access: `public`

Initializes this allocator over one fixed backing block.

```cpp
SharedReserveInitializationResult Initialize(
                const MemoryBlock& block
            ) noexcept
```

### `TearDown`

**Classification:** PUBLIC API

Tears down this allocator only when every shared allocation has been returned.

```cpp
SharedAllocatorTeardownResult TearDown() noexcept
```

### `Allocate`

**Classification:** PUBLIC API

Allocates the first suitably aligned contiguous span from the shared reserve.

```cpp
SharedAllocationResult Allocate(
                std::size_t byteCount,
                std::size_t alignment,
                SharedAllocation& allocation
            ) noexcept
```

### `Release`

**Classification:** PUBLIC API

Releases one previously allocated shared span and immediately coalesces free neighbors.

```cpp
SharedAllocationReleaseResult Release(
                const SharedAllocation& allocation
            ) noexcept
```

### `AddressOf`

**Classification:** PUBLIC API

Resolves the live payload address associated with one shared allocation token.

```cpp
void* AddressOf(
                const SharedAllocation& allocation
            ) noexcept
```

### `HasLiveAllocations`

**Classification:** PUBLIC API

Reports whether any allocated span remains live in the shared reserve.

```cpp
bool HasLiveAllocations() const noexcept
```

