# Shared Reserve Allocation

## Substitutable algorithm contract

`SharedReserveAllocationAlgorithm` is an Exclusive Composition capability. Bootstrap may select another conforming allocator at compile time.

`MemoryComposition<...>` injects `CoalescingFirstFitProvider` when no explicit algorithm provider is supplied. This is a compile-time default, not runtime fallback. The selected algorithm cannot change until the complete topology has been torn down and another runtime initialized.

## CoalescingFirstFitProvider

The default provider manages one fixed contiguous `MemoryBlock`.

Its metadata is entirely in-band inside the configured reserve. It performs no secondary allocation and requires no external descriptor table.

`SharedReserve<N,...>` represents the total configured reserve available to the allocator. N includes allocator metadata, alignment padding, and payload. There is no hidden EDP-Memory metadata allocation outside that reserve.

## Allocation

Allocation always searches from the beginning of the reserve and chooses the first free span that can provide the requested byte count at the requested power-of-two alignment.

Usable remainder becomes another free span. A remainder too small to represent usefully is absorbed into the allocation rather than creating an unusable fragment.

The allocator understands only bytes/alignment. It does not know object Types, per-Type quotas, constructors, or destructors.

## Release and coalescing

Release immediately coalesces adjacent free spans. The releasing caller pays that bounded synchronous execution cost; no maintenance worker/background compactor exists.

Live allocations are never moved.

## Exhaustion versus fragmentation

`SharedAllocationResult` distinguishes:

- `CapacityUnavailable` — aggregate free payload is insufficient;
- `ContiguousCapacityUnavailable` — sufficient aggregate free payload exists, but no individual suitably aligned contiguous span can satisfy the request.

That distinction is intentionally functional because it directly helps developers right-size reserve/capacity expectations.

No persistent fragmentation percentage, high-water mark, history, allocation count, or residency telemetry is retained in production v1. The allocator calculates only what an operation needs while it is running.

## Teardown

Allocator teardown succeeds only when no shared allocation remains live. Because topology teardown scans bounded allocator metadata rather than retaining extra historical/live counters solely for teardown speed, teardown may cost more execution time; this is intentional because teardown is exceptional and permanent RAM savings take priority.
