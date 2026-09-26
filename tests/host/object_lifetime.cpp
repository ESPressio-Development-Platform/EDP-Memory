#include <cassert>
#include <cstddef>
#include <utility>

#include "ESPressio_Memory.hpp"

namespace {

    struct Tracked final {
        int Value;
        bool* Destroyed;

        Tracked(int value, bool& destroyed) noexcept
            : Value(value), Destroyed(&destroyed) {}

        Tracked(Tracked&& other) noexcept
            : Value(other.Value), Destroyed(other.Destroyed) {
            other.Value = -1;
        }

        Tracked(const Tracked&) = delete;
        Tracked& operator=(const Tracked&) = delete;

        ~Tracked() noexcept {
            if (Destroyed != nullptr) {
                *Destroyed = true;
            }
        }
    };

}

int main() {
    namespace Memory = ESPressio::Memory;

    alignas(Tracked) std::byte sourceStorage[sizeof(Tracked)];
    alignas(Tracked) std::byte destinationStorage[sizeof(Tracked)];

    bool sourceDestroyed = false;
    auto* source = Memory::ObjectLifetime::Construct<Tracked>(
        sourceStorage,
        42,
        sourceDestroyed
    );

    assert(source != nullptr);
    assert(source->Value == 42);
    assert(!sourceDestroyed);

    auto* destination = Memory::ObjectLifetime::MoveConstruct<Tracked>(
        destinationStorage,
        *source
    );

    assert(destination != nullptr);
    assert(destination->Value == 42);
    assert(source->Value == -1);

    Memory::ObjectLifetime::Destroy(*source);
    assert(sourceDestroyed);

    bool destinationDestroyed = false;
    destination->Destroyed = &destinationDestroyed;
    Memory::ObjectLifetime::Destroy(*destination);
    assert(destinationDestroyed);

    return 0;
}
