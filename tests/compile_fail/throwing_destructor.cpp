#include <ESPressio_Memory.hpp>

struct ResourceProvider;

struct ThrowingDestructor final {
    ~ThrowingDestructor() noexcept(false) {}
};

using InvalidSpec = ESPressio::Memory::ObjectPoolSpec<
    ThrowingDestructor,
    ESPressio::Memory::DedicatedInstances<1U>
>;

int main() {
    static_cast<void>(sizeof(InvalidSpec));
    return 0;
}
