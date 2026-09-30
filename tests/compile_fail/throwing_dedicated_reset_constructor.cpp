#include <ESPressio_Memory.hpp>

struct ThrowingValue final {
    explicit ThrowingValue(int) {}
    ~ThrowingValue() noexcept = default;
};

struct DummyRuntime final {
    template<class TObject, class TObjectPool, class... TArguments>
    ESPressio::Memory::DedicatedObjectPoolResetResult ResetDedicatedObject(
        TObjectPool&,
        typename TObjectPool::DedicatedIndex,
        TArguments&&...
    ) noexcept {
        return ESPressio::Memory::DedicatedObjectPoolResetResult::Succeeded;
    }
};

using Spec = ESPressio::Memory::ObjectPoolSpec<
    ThrowingValue,
    ESPressio::Memory::DedicatedInstances<1U>
>;

using Pool = ESPressio::Memory::ObjectPool<
    ThrowingValue,
    Spec,
    DummyRuntime
>;

int main() {
    Pool pool;
    auto index = Pool::DedicatedIndex::FromUnchecked(0U);

    static_cast<void>(
        pool.ResetDedicated(
            index,
            1
        )
    );

    return 0;
}
