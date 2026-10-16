// G1 #16 regression: detached cancellation cannot allow premature teardown.
// The runtime must retain teardown ownership until each cancelled acquisition exits.
#define main EDP_MEMORY_PRIOR_HOST_SUITE
#include "main.cpp"
#undef main

class PausedSignalProvider final : public Framework::Provider<
    Platform::Domain,
    Framework::Offers<
        Framework::Offer<
            Platform::Synchronization::Signal,
            Framework::PropertyValue<Platform::Synchronization::SignalWaitResolutionNanoseconds, 1ULL>,
            Framework::PropertyValue<Platform::Synchronization::SignalSupportsInterruptNotification, false>
        >
    >
> {
public:
    PausedSignalProvider() noexcept = default;
    PausedSignalProvider(const PausedSignalProvider&) = delete;
    PausedSignalProvider& operator =(const PausedSignalProvider&) = delete;
    PausedSignalProvider(PausedSignalProvider&&) = delete;
    PausedSignalProvider& operator =(PausedSignalProvider&&) = delete;
    static inline std::atomic<bool> Entered{false};
    static inline std::atomic<bool> AllowReturn{false};

    Platform::Synchronization::SignalNotifyResult Notify() noexcept {
        return Platform::Synchronization::SignalNotifyResult::Signaled;
    }
    Platform::Synchronization::SignalNotifyResult NotifyFromInterrupt() noexcept {
        return Platform::Synchronization::SignalNotifyResult::UnsupportedInterruptContext;
    }
    Platform::Synchronization::SignalWaitResult Wait(
        Platform::Synchronization::WaitTimeout
    ) noexcept {
        Entered.store(true, std::memory_order_release);
        while (!AllowReturn.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        return Platform::Synchronization::SignalWaitResult::Signaled;
    }
};

int main() {
    using DelayedRuntime = Memory::MemoryRuntime<
        Topology, MemoryComposition, TestSupport::TestMutexProvider, PausedSignalProvider
    >;
    Resource resource;
    TestSupport::TestMutexProvider mutex;
    MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
    DelayedRuntime runtime(mutex, allocator, resource);
    Memory::MemoryTopologyInitializationFailure failure{};
    assert(runtime.Initialize(failure) == Memory::MemoryTopologyInitializationResult::Succeeded);
    auto& pool = runtime.ObjectPoolFor<TestSupport::DedicatedOnlyObject>();
    DelayedRuntime::ObjectPoolType<TestSupport::DedicatedOnlyObject>::LeaseType owner{};
    assert(pool.Acquire(owner, Platform::Synchronization::WaitTimeout::NoWait(), 1) ==
           Memory::ObjectPoolAcquisitionResult::Succeeded);
    std::atomic<Memory::ObjectPoolAcquisitionResult> waiterResult{
        Memory::ObjectPoolAcquisitionResult::CapacityUnavailable
    };
    std::thread waiter([&]() {
        DelayedRuntime::ObjectPoolType<TestSupport::DedicatedOnlyObject>::LeaseType lease{};
        waiterResult.store(
            pool.Acquire(lease, Platform::Synchronization::WaitTimeout::Forever(), 2),
            std::memory_order_release
        );
    });
    while (!PausedSignalProvider::Entered.load(std::memory_order_acquire)) {
        std::this_thread::yield();
    }
    assert(runtime.CancelPendingAcquisitions() ==
           Memory::PendingAcquisitionCancellationResult::Cancelled);
    assert(owner.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
    const auto teardown = runtime.TearDown();
    const bool safe = teardown == Memory::MemoryTopologyTeardownResult::PendingAcquisitionsRemain;
    std::fprintf(stderr, "G1 cancelled waiter: teardown=%u pending=%d\n",
                 static_cast<unsigned>(teardown), safe);
    PausedSignalProvider::AllowReturn.store(true, std::memory_order_release);
    waiter.join();
    assert(waiterResult.load(std::memory_order_acquire) ==
           Memory::ObjectPoolAcquisitionResult::TopologyUnavailable);
    if (safe) {
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::Succeeded);
    }
    return safe ? 0 : 43;
}
