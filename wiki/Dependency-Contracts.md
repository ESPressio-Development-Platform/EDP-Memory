# Dependency Contracts

EDP-Memory depends on **EDP-System** and **EDP-Platform**.

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

## MemoryResource provider contract

Every provider matched by the Memory Composition must satisfy `MemoryResourceProviderTraits`: it must offer `MemoryResource`, implement noexcept `Allocate(size,alignment,block)` returning `MemoryAllocationResult`, and noexcept `Release(block)` returning `MemoryReleaseResult`.

The topology's default resource, shared-reserve resource and any explicitly selected ObjectPool resource must all be present in the selected Memory Composition.

## ByteOperations internal provider API

`Memory::Detail::ByteOperationsProviderTraits` validates the cross-repository ByteOperations contract and is consumed by BoundedTypes, Localisation, Persistence concrete providers and Security implementations.

## Lifetime/resource boundary

Bootstrap owns all provider instances. MemoryRuntime owns topology bookkeeping/pools and borrows the selected resource/synchronization providers.

> Dependency contract audit baseline: `c207fa8f6ee596ac8791b7d2c88509c372fda418` (`main`).
