# Memory Demos

`EDP-Memory` supplies **five** focused examples:

- `memory-operations-basic` — stateless raw ByteOperations plus explicitly allocated/released aligned MemoryResource, imported through **both** corresponding narrow Portable public root headers.
- `object-pool-basic` — finite compile-time topology, dedicated Object Pool and typed lease/index ownership.
- `indexed-object-pool-basic` — compact one-byte dedicated-slot identity, safe nothrow reset and stable address (no shared overflow).
- `shared-overflow-basic` — dedicated-first allocation, finite shared reserve, capacity backpressure.
- `topology-teardown-basic` — teardown refusal with a live lease, completed teardown and explicit reinitialization.

Every family has an Arduino IDE sketch and two PlatformIO ESP32 applications: Arduino-ESP32 and native ESP-IDF. All three variants use **only the directly needed concrete providers**: the Standard C++ `EDP-Platform-Portable` MemoryResource via `<ESPressio_Platform_Portable_MemoryResource.hpp>`, and `EDP-Platform-FreeRTOS` Mutex/Signal via `<ESPressio_Platform_FreeRTOS.hpp>` where an Object Pool requires synchronization. ByteOperations alone uses the existing narrow Portable ByteOperations root. The applications must install/declare all selected concrete libraries explicitly; EDP-Memory's production library retains only abstract dependencies System, Platform and BoundedTopology.

The complete `<ESPressio_Platform_Portable.hpp>` umbrella is deliberately **not** used by ESP32 examples: it also imports `AtomicWord32` with a lock-free Standard C++ constraint that these targets may not satisfy, and exception-using Standard C++ synchronization that can be incompatible with native ESP-IDF builds compiled without exceptions. No SDK contract or static assertion is weakened by avoiding unrelated provider imports.

For PlatformIO, build within the desired `PlatformIO_Arduino` or `PlatformIO_ESP-IDF` directory with `pio run`. For Arduino IDE, install the declared EDP dependencies and compile the `Arduino_IDE` sketch with the intended ESP32 core and C++20-compatible toolchain. A successful **build** is not evidence of hardware execution or runtime behaviour. These demos exercise NoWait/short-lived ownership; direct native finite-wait safety remains the separately tracked [EDP-Memory #16](https://github.com/ESPressio-Development-Platform/EDP-Memory/issues/16).

## C++ language baseline

C++20 is required. PlatformIO targets use the maintained PIOArduino ESP32 platform package. All library versions remain `0.1.0` during the approved reintegration tranche.
