# Byte Operations and Memory Resources

## Purpose

`EDP-Memory` owns the ESPressio-language contracts for raw memory operations. It deliberately does not embed Standard C/C++, ESP-IDF, Arduino, FreeRTOS, or other SDK allocation/copy vocabulary into reusable Memory code.

## ByteOperations

`ByteOperations` is an Exclusive Composition capability with four operations:

- `CopyBytes(destination, source, byteCount)` — source and destination ranges must not overlap.
- `MoveBytes(destination, source, byteCount)` — overlapping ranges are permitted.
- `FillBytes(destination, value, byteCount)` — writes one explicit `std::uint8_t` value.
- `CompareBytes(left, right, byteCount)` — unsigned-byte lexicographical ordering returning `ByteComparison::{Less,Equal,Greater}`.

All operations are `noexcept`, allocation-free, and synchronization-free. A zero byte count is valid and performs no memory access. For non-zero operations, readable/writable range validity is a caller precondition; EDP-Memory does not pretend that arbitrary pointers can be validated reliably at runtime.

`ByteOperations` does not create, assign, relocate, or destroy C++ objects. Raw byte copying is only valid when the caller's object-representation rules permit it.

There is no separate ordinary `ZeroBytes` operation; ordinary zeroing is `FillBytes(..., 0, ...)`. Secure erasure is not implied and belongs to a separate future security contract if required.

## MemoryResource

`MemoryResource` is a Shared Composition capability because one application may expose multiple simultaneous raw memory sources.

A successful allocation publishes one `MemoryBlock` containing:

- `Address` — first writable byte;
- `Size` — actual usable byte count owned by the allocation;
- `Alignment` — alignment satisfied by `Address`.

`Alignment` is always present. It is not hidden behind a compiler definition, so `MemoryBlock` layout is stable across consumers/providers.

`Allocate(byteCount, alignment, block)` requires non-zero size and a non-zero power-of-two alignment. Failure leaves the caller's output block unchanged. `MemoryAllocationResult` distinguishes invalid size, invalid alignment, resource exhaustion, and provider failure.

`Release(block)` must be called on the same provider instance that allocated the block. `MemoryBlock` stores no provider pointer, resource identifier, RAII behavior, or object-lifetime semantics.

## Concrete provider ownership

Concrete ownership follows the underlying system API:

- Standard C/C++ byte/allocation APIs -> `EDP-Platform-Portable`;
- ESP-IDF heap-capability / PSRAM APIs -> `EDP-Platform-ESP-IDF`;
- Arduino-specific APIs -> `EDP-Platform-Arduino` only when the concrete genuinely depends on Arduino;
- FreeRTOS-specific APIs -> `EDP-Platform-FreeRTOS` only when the concrete genuinely depends on FreeRTOS.

A Platform repository does not receive a Memory provider merely for symmetry. If an implementation ultimately calls Standard C/C++, the concrete belongs in `EDP-Platform-Portable`.

## PSRAM / fallback boundary

EDP-Memory does not know PSRAM names, ESP-IDF capability flags, or physical placement details.

Where one concrete provider implements “external preferred, internal fallback”, that policy remains inside the provider. This preserves one coherent allocate/release owner and avoids storing provider identity in every `MemoryBlock`.

If an application requires guaranteed placement rather than preference, Bootstrap selects a provider whose own documented contract guarantees that placement.
