# Compiler Definitions and Conditional Compilation

No repository-defined compiler definitions were found in the audited PlatformIO/CMake/SDK configuration. Topology, pool capacities, shared reserve size and provider selection are compile-time C++ declarations.

External platform/compiler macros are not EDP configuration knobs unless this repository defines them.

> Audited against `413906daac60991e622e96689d480656fcc48259` on `main` across 9 build-configuration file(s).

## Event V1 indexed-pool tranche

No new EDP-Memory compiler definition or conditional-compilation switch is introduced. Dedicated indexed ownership remains entirely governed by C++ compile-time topology declarations (`DedicatedInstances<N>`) and ordinary provider selection. Platform SDK macros remain external to EDP-Memory.
