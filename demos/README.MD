# Demos

`EDP-Memory` includes four focused demonstration families:

- `memory-operations-basic` — consumes ByteOperations and MemoryResource through application-selected Portable providers;
- `object-pool-basic` — initializes one compile-time topology and demonstrates dedicated Object Pool acquisition / explicit lease release;
- `shared-overflow-basic` — demonstrates dedicated-first allocation, one bounded shared-overflow instance, and explicit capacity failure;
- `topology-teardown-basic` — demonstrates teardown refusal while a lease is live, complete teardown, and reinitialization.

Each logical demo provides Arduino IDE, PlatformIO Arduino and PlatformIO ESP-IDF surfaces as required by ESPressio policy.

The demos deliberately use `EDP-Platform-Portable` as the concrete provider selection for Standard C++ Memory operations plus Mutex/Signal coordination. They contain no diagnostic framework, background worker, service locator, or hidden allocation policy.

## C++ Language Baseline

All PlatformIO-compatible demos require C++20 and explicitly select the maintained PIOArduino ESP32 platform package.

## Indexed dedicated ownership

The `object-pool-basic` demo now exercises both ownership forms: ordinary move-only `ObjectPoolLease` and compact `ObjectPool::DedicatedIndex` acquisition/access/release. The indexed path is dedicated-only and never consumes shared overflow.
### `indexed-object-pool-basic`

Demonstrates the compact dedicated-only ownership path: acquire one nothrow object into exact dedicated capacity, retain only the one-byte strong `DedicatedIndex`, resolve the stable live object, release the slot, and observe index invalidation. It intentionally performs no shared fallback and no wait.
