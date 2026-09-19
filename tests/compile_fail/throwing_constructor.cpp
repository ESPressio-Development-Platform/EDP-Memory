#include <ESPressio_Memory.hpp>

struct ThrowingValue final {
    explicit ThrowingValue(int) {}
    ~ThrowingValue() noexcept = default;
};

struct DummyRuntime final {
    template<class TObject, class TObjectPool, class TObjectLease, class... TArguments>
    ESPressio::Memory::ObjectPoolAcquisitionResult AcquireObject(
        TObjectPool&,
        TObjectLease&,
        ESPressio::Platform::Synchronization::WaitTimeout,
        TArguments&&...
    ) noexcept {
        return ESPressio::Memory::ObjectPoolAcquisitionResult::Succeeded;
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
    Pool::LeaseType lease;

    static_cast<void>(
        pool.Acquire(
            lease,
            ESPressio::Platform::Synchronization::WaitTimeout::NoWait(),
            1
        )
    );

    return 0;
}
