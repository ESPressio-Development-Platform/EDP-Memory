#include <ESPressio_Memory.hpp>

struct ResourceProvider;
struct Value final { ~Value() noexcept = default; };

using Spec = ESPressio::Memory::ObjectPoolSpec<
    Value,
    ESPressio::Memory::DedicatedInstances<1U>
>;

using InvalidTopology = ESPressio::Memory::MemoryTopology<
    ResourceProvider,
    ESPressio::Memory::SharedReserve<0U, ResourceProvider>,
    Spec,
    Spec
>;

int main() {
    static_cast<void>(sizeof(InvalidTopology));
    return 0;
}
