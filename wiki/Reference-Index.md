# Reference Index

Every production header under `src/` has a source-derived reference page. Public topology/pool contracts, provider contracts and private bookkeeping structures are classified separately.

| Source header | Classification | Reference |
|---|---|---|
| `src/ESPressio_Memory.hpp` | PUBLIC ENTRY POINT | [open](Reference-ESPressio-Memory) |
| `src/memory/ByteOperationsContract.hpp` | INTERNAL PROVIDER API | [open](Reference-memory-ByteOperationsContract) |
| `src/memory/CoalescingFirstFitProvider.hpp` | PUBLIC API | [open](Reference-memory-CoalescingFirstFitProvider) |
| `src/memory/detail/ObjectPoolState.hpp` | PRIVATE IMPLEMENTATION | [open](Reference-memory-detail-ObjectPoolState) |
| `src/memory/detail/ObjectPoolToken.hpp` | PRIVATE IMPLEMENTATION | [open](Reference-memory-detail-ObjectPoolToken) |
| `src/memory/detail/TopologyTraits.hpp` | PRIVATE IMPLEMENTATION | [open](Reference-memory-detail-TopologyTraits) |
| `src/memory/detail/WaitRequest.hpp` | PRIVATE IMPLEMENTATION | [open](Reference-memory-detail-WaitRequest) |
| `src/memory/MemoryComposition.hpp` | PUBLIC API | [open](Reference-memory-MemoryComposition) |
| `src/memory/MemoryCompositionDefaults.hpp` | INTERNAL COMPOSITION API | [open](Reference-memory-MemoryCompositionDefaults) |
| `src/memory/MemoryResourceContract.hpp` | INTERNAL PROVIDER API | [open](Reference-memory-MemoryResourceContract) |
| `src/memory/MemoryRuntime.hpp` | PUBLIC API | [open](Reference-memory-MemoryRuntime) |
| `src/memory/MemoryTopology.hpp` | PUBLIC API | [open](Reference-memory-MemoryTopology) |
| `src/memory/MemoryTypes.hpp` | PUBLIC API | [open](Reference-memory-MemoryTypes) |
| `src/memory/ObjectLifetime.hpp` | PUBLIC API | [open](Reference-memory-ObjectLifetime) |
| `src/memory/ObjectPool.hpp` | PUBLIC API | [open](Reference-memory-ObjectPool) |
| `src/memory/ObjectPoolable.hpp` | PUBLIC API | [open](Reference-memory-ObjectPoolable) |
| `src/memory/ObjectPoolConfiguration.hpp` | PUBLIC API | [open](Reference-memory-ObjectPoolConfiguration) |
| `src/memory/ObjectPoolLease.hpp` | PUBLIC API | [open](Reference-memory-ObjectPoolLease) |
| `src/memory/OwnershipTransfer.hpp` | PUBLIC API | [open](Reference-memory-OwnershipTransfer) |
| `src/memory/SharedReserveAllocationContract.hpp` | INTERNAL PROVIDER API | [open](Reference-memory-SharedReserveAllocationContract) |

> Latest deep-pass baseline: `35475501654b39cc2b8a9f2032a00e9d30fa3058`.

## Event V1 indexed-pool tranche

No production header was added by the indexed-pool feature. The changed declarations are documented in the existing `ObjectPool`, `MemoryRuntime`, `MemoryTypes`, and `detail/ObjectPoolState` reference pages. This tranche also closed the pre-existing reference omissions for `ObjectLifetime.hpp` and `OwnershipTransfer.hpp`, restoring exhaustive one-page-per-source coverage. Exact source baselines are refreshed after the canonical branch commit.
