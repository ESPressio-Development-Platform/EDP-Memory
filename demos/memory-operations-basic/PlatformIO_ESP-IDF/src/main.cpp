#include <array>
#include <cstdint>

#include <ESPressio_Memory.hpp>
#include <ESPressio_Platform_Portable.hpp>

namespace Demo {

    /// Exercises EDP-Memory abstractions through the application-selected Portable providers.
    int Run() noexcept {
        ESPressio::Platform::Portable::Memory::ByteOperationsProvider bytes;
        ESPressio::Platform::Portable::Memory::MemoryResourceProvider resource;

        std::array<std::uint8_t, 4U> source{1U, 2U, 3U, 4U};
        std::array<std::uint8_t, 4U> target{};

        bytes.CopyBytes(
            target.data(),
            source.data(),
            source.size()
        );

        if (
            bytes.CompareBytes(
                target.data(),
                source.data(),
                source.size()
            ) != ESPressio::Memory::ByteComparison::Equal
        ) {
            return 1;
        }

        ESPressio::Memory::MemoryBlock block{};

        if (
            resource.Allocate(
                128U,
                32U,
                block
            ) != ESPressio::Memory::MemoryAllocationResult::Succeeded
        ) {
            return 2;
        }

        return resource.Release(block) ==
            ESPressio::Memory::MemoryReleaseResult::Released ? 0 : 3;
    }

} // Demo

extern "C" void app_main() {
    static_cast<void>(Demo::Run());
}
