#pragma once

#include <ESPressio_System.hpp>

namespace ESPressio::Memory {

    namespace Framework = ESPressio::System::CompositionFramework;

    /// Composition domain containing deterministic Memory capabilities.
    struct Domain final : Framework::Domain {};


    /// Exclusive capability supplying raw byte-copy, move, fill, and comparison operations.
    struct ByteOperations final : Framework::ExclusiveCapability<Domain> {};


    /// Shared capability supplying one raw aligned memory resource.
    struct MemoryResource final : Framework::SharedCapability<Domain> {};


    /// Exclusive capability supplying the shared-reserve suballocation algorithm.
    struct SharedReserveAllocationAlgorithm final : Framework::ExclusiveCapability<Domain> {};

} // ESPressio::Memory
