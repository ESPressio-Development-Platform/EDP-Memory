# Dependency Contracts

EDP-Memory depends on **EDP-System**, **EDP-Platform**, and **EDP-BoundedTopology**.

For Arduino IDE / Arduino CLI compatibility, EDP-Memory enters ESPressio dependencies through their public umbrella headers rather than cross-library internal source paths.

## EDP-System

The Memory domain and all provider selection use the EDP-System Composition Framework.

Within the Memory domain:

- `SharedReserveAllocationRequirement` requires **exactly one** same-domain `SharedReserveAllocationAlgorithm` provider;
- `MemoryResourceRequirement` requires **at least one** same-domain `MemoryResource` provider.

`MemoryComposition<...>` injects `CoalescingFirstFitProvider` when Bootstrap supplies no explicit shared-reserve allocator.

## EDP-Platform synchronization

`MemoryRuntime<TTopology,TMemoryComposition,TMutexProvider,TSignalProvider>` receives concrete Platform synchronization provider types directly.

- `TMutexProvider` is validated by `Platform::Synchronization::Detail::MutexProviderTraits` and protects topology bookkeeping.
- `TSignalProvider` is validated by `SignalProviderTraits` and is instantiated for targeted blocked-waiter notification.

These are direct provider-trait contracts rather than Composition Requirements.

## EDP-BoundedTopology

`ObjectPool` consumes the public `BoundedIndex` value Type to represent the semantic identity of one slot in the exact compile-time dedicated capacity. This is a mandatory representation dependency introduced by the Event V1 indexed-pool tranche.

BoundedTopology owns no backing allocation, object lifetime, occupancy, synchronization or provider selection. EDP-Memory remains the owner of all those responsibilities. The dependency direction is acyclic because BoundedTopology has no dependency on EDP-Memory.

## MemoryResource provider contract

Every provider matched by the Memory Composition must satisfy `MemoryResourceProviderTraits`: it must offer `MemoryResource`, implement noexcept `Allocate(size,alignment,block)` returning `MemoryAllocationResult`, and noexcept `Release(block)` returning `MemoryReleaseResult`.

The topology's default resource, shared-reserve resource and any explicitly selected ObjectPool resource must all be present in the selected Memory Composition.

## ByteOperations internal provider API

`Memory::Detail::ByteOperationsProviderTraits` validates the cross-repository ByteOperations contract and is consumed by BoundedTypes, Localisation, Persistence concrete providers and Security implementations.

## Lifetime/resource boundary

Bootstrap owns all provider instances. MemoryRuntime owns topology bookkeeping/pools and borrows the selected resource/synchronization providers. Indexed dedicated consumers retain only a strong slot index; they do not own the backing allocation.
