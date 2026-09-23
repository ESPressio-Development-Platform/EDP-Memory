#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <new>
#include <thread>

#include "ESPressio_Memory.hpp"

namespace TestSupport {

    namespace Memory = ESPressio::Memory;
    namespace Platform = ESPressio::Platform;
    namespace Framework = ESPressio::System::CompositionFramework;

    /// Heap-backed deterministic test resource with injectable allocation/release failures.
    class TestMemoryResourceProvider final : public Framework::Provider<
        Memory::Domain,
        Framework::Offers<
            Framework::Offer<Memory::MemoryResource>
        >
    > {

        private:

            // Failure injection and accounting.

            /// Allocation ordinal at which failure should be injected, or zero when disabled.
            std::size_t _failAllocationOrdinal = 0U;

            /// Number of allocation requests observed by this provider.
            std::size_t _allocationCount = 0U;

            /// Indicates that release should report provider failure.
            bool _failRelease = false;

        public:

            // Test control.

            /// Injects ResourceExhausted for the specified one-based allocation ordinal.
            void FailAllocationAt(
                std::size_t ordinal
            ) noexcept {
                _failAllocationOrdinal = ordinal;
            }

            /// Enables or disables release failure injection.
            void FailRelease(
                bool enabled
            ) noexcept {
                _failRelease = enabled;
            }


            // MemoryResource operations.

            /// Allocates aligned host storage without throwing.
            Memory::MemoryAllocationResult Allocate(
                std::size_t byteCount,
                std::size_t alignment,
                Memory::MemoryBlock& block
            ) noexcept {
                if (byteCount == 0U) { return Memory::MemoryAllocationResult::InvalidSize; }
                if (alignment == 0U || (alignment & (alignment - 1U)) != 0U) {
                    return Memory::MemoryAllocationResult::InvalidAlignment;
                }

                ++_allocationCount;

                if (_failAllocationOrdinal != 0U && _allocationCount == _failAllocationOrdinal) {
                    return Memory::MemoryAllocationResult::ResourceExhausted;
                }

                void* allocation = ::operator new(
                    byteCount,
                    std::align_val_t(alignment),
                    std::nothrow
                );

                if (allocation == nullptr) { return Memory::MemoryAllocationResult::ResourceExhausted; }

                Memory::MemoryBlock candidate;
                candidate.Address = allocation;
                candidate.Size = byteCount;
                candidate.Alignment = alignment;
                block = candidate;

                return Memory::MemoryAllocationResult::Succeeded;
            }

            /// Releases aligned host storage previously allocated by this provider.
            Memory::MemoryReleaseResult Release(
                const Memory::MemoryBlock& block
            ) noexcept {
                if (block.Address == nullptr || block.Alignment == 0U) {
                    return Memory::MemoryReleaseResult::InvalidBlock;
                }

                if (_failRelease) { return Memory::MemoryReleaseResult::ProviderFailure; }

                ::operator delete(
                    block.Address,
                    std::align_val_t(block.Alignment)
                );

                return Memory::MemoryReleaseResult::Released;
            }

    };


    /// Standard-library-backed Mutex provider used only by host tests.
    class TestMutexProvider final : public Framework::Provider<
        Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                Platform::Synchronization::Mutex,
                Framework::PropertyValue<
                    Platform::Synchronization::MutexWaitResolutionNanoseconds,
                    1ULL
                >
            >
        >
    > {

        private:

            // Native test mutex.

            /// Timed mutex implementing the test provider.
            std::timed_mutex _mutex;

        public:

            /// Acquires the mutex according to the requested Platform wait policy.
            Platform::Synchronization::LockAcquireResult Acquire(
                Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                try {
                    if (timeout.IsForever()) {
                        _mutex.lock();
                        return Platform::Synchronization::LockAcquireResult::Acquired;
                    }

                    if (timeout.IsNoWait()) {
                        return _mutex.try_lock()
                            ? Platform::Synchronization::LockAcquireResult::Acquired
                            : Platform::Synchronization::LockAcquireResult::TimedOut;
                    }

                    return _mutex.try_lock_for(
                        std::chrono::nanoseconds(
                            timeout.Nanoseconds()
                        )
                    )
                        ? Platform::Synchronization::LockAcquireResult::Acquired
                        : Platform::Synchronization::LockAcquireResult::TimedOut;
                } catch (...) {
                    return Platform::Synchronization::LockAcquireResult::ProviderFailure;
                }
            }

            /// Releases the mutex owned by the current host execution context.
            Platform::Synchronization::LockReleaseResult Release() noexcept {
                try {
                    _mutex.unlock();
                    return Platform::Synchronization::LockReleaseResult::Released;
                } catch (...) {
                    return Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }
            }

    };


    /// Targeted condition-variable Signal provider instantiated on each blocked caller stack.
    class TestSignalProvider final : public Framework::Provider<
        Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                Platform::Synchronization::Signal,
                Framework::PropertyValue<
                    Platform::Synchronization::SignalWaitResolutionNanoseconds,
                    1ULL
                >,
                Framework::PropertyValue<
                    Platform::Synchronization::SignalSupportsInterruptNotification,
                    false
                >
            >
        >
    > {

        private:

            // Latched signal state.

            /// Mutex protecting the latch.
            std::mutex _mutex;

            /// Condition variable waking this request's single waiter.
            std::condition_variable _condition;

            /// Latched notification state.
            bool _signaled = false;

        public:

            /// Notifies the blocked waiter and latches the signal if it has not waited yet.
            Platform::Synchronization::SignalNotifyResult Notify() noexcept {
                try {
                    {
                        std::lock_guard<std::mutex> lock(_mutex);
                        _signaled = true;
                    }

                    _condition.notify_one();
                    return Platform::Synchronization::SignalNotifyResult::Signaled;
                } catch (...) {
                    return Platform::Synchronization::SignalNotifyResult::ProviderFailure;
                }
            }

            /// Reports that the host test signal does not model interrupt-context notification.
            Platform::Synchronization::SignalNotifyResult NotifyFromInterrupt() noexcept {
                return Platform::Synchronization::SignalNotifyResult::UnsupportedInterruptContext;
            }

            /// Waits for the targeted notification according to the requested Platform wait policy.
            Platform::Synchronization::SignalWaitResult Wait(
                Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                try {
                    std::unique_lock<std::mutex> lock(_mutex);

                    const auto signaled = [this]() noexcept {
                        return _signaled;
                    };

                    if (timeout.IsForever()) {
                        _condition.wait(
                            lock,
                            signaled
                        );
                    } else if (timeout.IsNoWait()) {
                        if (!signaled()) { return Platform::Synchronization::SignalWaitResult::TimedOut; }
                    } else if (
                        !_condition.wait_for(
                            lock,
                            std::chrono::nanoseconds(
                                timeout.Nanoseconds()
                            ),
                            signaled
                        )
                    ) {
                        return Platform::Synchronization::SignalWaitResult::TimedOut;
                    }

                    _signaled = false;
                    return Platform::Synchronization::SignalWaitResult::Signaled;
                } catch (...) {
                    return Platform::Synchronization::SignalWaitResult::ProviderFailure;
                }
            }

    };


    /// Tracks pooled construction/destruction and exposes one payload value.
    class PooledObject final {

        private:

            // Payload.

            /// Value supplied to the noexcept constructor.
            int _value;

        public:

            // Lifetime counters.

            /// Number of currently live PooledObject instances.
            static std::atomic<int> LiveCount;

            /// Constructs one pooled value without allocating memory.
            explicit PooledObject(
                int value
            ) noexcept :
                _value(value) {
                ++LiveCount;
            }

            /// Records destruction of one pooled value.
            ~PooledObject() noexcept {
                --LiveCount;
            }

            /// Returns the test payload.
            int Value() const noexcept {
                return _value;
            }

    };

    std::atomic<int> PooledObject::LiveCount{0};


    /// Separate Type used to validate uncapped shared-overflow eligibility.
    class UncappedSharedObject final {

        private:

            // Payload.

            /// Value supplied at construction.
            int _value;

        public:

            /// Constructs one uncapped shared-overflow test object.
            explicit UncappedSharedObject(
                int value
            ) noexcept :
                _value(value) {
            }

            /// Provides nothrow destruction required by ObjectPool.
            ~UncappedSharedObject() noexcept = default;

            /// Returns the test payload.
            int Value() const noexcept {
                return _value;
            }

    };


    /// Separate dedicated-only Type used to validate blocking handoff and cancellation.
    class DedicatedOnlyObject final {

        private:

            // Payload.

            /// Value supplied at construction.
            int _value;

        public:

            /// Constructs one dedicated-only pooled object.
            explicit DedicatedOnlyObject(
                int value
            ) noexcept :
                _value(value) {
            }

            /// Provides nothrow destruction required by ObjectPool.
            ~DedicatedOnlyObject() noexcept = default;

            /// Returns the test payload.
            int Value() const noexcept {
                return _value;
            }

    };

} // TestSupport

namespace {

    namespace Memory = ESPressio::Memory;
    namespace Platform = ESPressio::Platform;

    using Resource = TestSupport::TestMemoryResourceProvider;
    using MemoryComposition = Memory::MemoryComposition<Resource>;
    using Topology = Memory::MemoryTopology<
        Resource,
        Memory::SharedReserve<512U, Resource>,
        Memory::ObjectPoolSpec<
            TestSupport::PooledObject,
            Memory::DedicatedInstances<1U>,
            Memory::SharedOverflow<1U>
        >,
        Memory::ObjectPoolSpec<
            TestSupport::DedicatedOnlyObject,
            Memory::DedicatedInstances<1U>,
            Memory::NoSharedOverflow
        >,
        Memory::ObjectPoolSpec<
            TestSupport::UncappedSharedObject,
            Memory::DedicatedInstances<0U>,
            Memory::SharedOverflow<0U>
        >
    >;
    using Runtime = Memory::MemoryRuntime<
        Topology,
        MemoryComposition,
        TestSupport::TestMutexProvider,
        TestSupport::TestSignalProvider
    >;
    using PooledObjectPool = Runtime::ObjectPoolType<TestSupport::PooledObject>;
    using PooledLease = PooledObjectPool::LeaseType;

    static_assert(
        std::is_same_v<
            MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique>,
            Memory::CoalescingFirstFitProvider
        >,
        "MemoryComposition must inject CoalescingFirstFitProvider when Bootstrap supplies no alternative"
    );

    static_assert(
        sizeof(Memory::MemoryBlock) == sizeof(void*) + (2U * sizeof(std::size_t)),
        "MemoryBlock must remain exactly address, size, and alignment"
    );

    static_assert(
        sizeof(PooledLease) == sizeof(void*) + sizeof(std::size_t),
        "ObjectPoolLease must remain one pool pointer plus one compact token"
    );

    static_assert(
        !std::is_copy_constructible_v<PooledLease> &&
        !std::is_copy_assignable_v<PooledLease> &&
        std::is_move_constructible_v<PooledLease> &&
        std::is_move_assignable_v<PooledLease>,
        "ObjectPoolLease must provide unique movable ownership"
    );


    /// Validates first-fit alignment, fragmentation classification, and immediate coalescing.
    void TestSharedAllocator() {
        alignas(std::max_align_t) std::byte storage[256U]{};
        Memory::MemoryBlock block;
        block.Address = storage;
        block.Size = sizeof(storage);
        block.Alignment = alignof(std::max_align_t);

        Memory::CoalescingFirstFitProvider allocator;
        assert(
            allocator.Initialize(block) ==
            Memory::SharedReserveInitializationResult::Succeeded
        );

        Memory::SharedAllocation first;
        Memory::SharedAllocation second;
        Memory::SharedAllocation third;

        assert(allocator.Allocate(
            48U,
            8U,
            first
        ) == Memory::SharedAllocationResult::Succeeded);
        assert(allocator.Allocate(
            48U,
            8U,
            second
        ) == Memory::SharedAllocationResult::Succeeded);
        assert(allocator.Allocate(
            48U,
            8U,
            third
        ) == Memory::SharedAllocationResult::Succeeded);

        assert(allocator.Release(first) == Memory::SharedAllocationReleaseResult::Released);
        assert(allocator.Release(third) == Memory::SharedAllocationReleaseResult::Released);

        Memory::SharedAllocation fragmented;
        assert(allocator.Allocate(
            140U,
            8U,
            fragmented
        ) == Memory::SharedAllocationResult::ContiguousCapacityUnavailable);

        assert(allocator.Release(second) == Memory::SharedAllocationReleaseResult::Released);
        assert(allocator.Allocate(
            140U,
            8U,
            fragmented
        ) == Memory::SharedAllocationResult::Succeeded);
        assert(allocator.Release(fragmented) == Memory::SharedAllocationReleaseResult::Released);

        Memory::SharedAllocation aligned;
        assert(allocator.Allocate(
            13U,
            64U,
            aligned
        ) == Memory::SharedAllocationResult::Succeeded);
        assert(
            reinterpret_cast<std::uintptr_t>(allocator.AddressOf(aligned)) % 64U == 0U
        );
        assert(allocator.Release(aligned) == Memory::SharedAllocationReleaseResult::Released);
        assert(!allocator.HasLiveAllocations());
        assert(allocator.TearDown() == Memory::SharedAllocatorTeardownResult::Succeeded);
    }


    /// Validates dedicated-first acquisition, shared quota, RAII, explicit release, and teardown/reinitialize.
    void TestObjectPoolLifecycle() {
        Resource resource;
        TestSupport::TestMutexProvider mutex;
        MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );
        Memory::MemoryTopologyInitializationFailure failure;

        assert(
            runtime.Initialize(failure) ==
            Memory::MemoryTopologyInitializationResult::Succeeded
        );
        assert(runtime.IsInitialized());

        auto& pool = runtime.ObjectPoolFor<TestSupport::PooledObject>();
        PooledLease first;
        PooledLease second;
        PooledLease unavailable;

        assert(pool.Acquire(
            first,
            Platform::Synchronization::WaitTimeout::NoWait(),
            10
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(pool.Acquire(
            second,
            Platform::Synchronization::WaitTimeout::NoWait(),
            20
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(TestSupport::PooledObject::LiveCount.load() == 2);
        assert(first->Value() == 10);
        assert(second->Value() == 20);

        const auto* firstAddress = first.Get();
        assert(firstAddress == first.Get());

        assert(pool.Acquire(
            unavailable,
            Platform::Synchronization::WaitTimeout::NoWait(),
            30
        ) == Memory::ObjectPoolAcquisitionResult::CapacityUnavailable);
        assert(unavailable.IsEmpty());
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::LiveObjectsRemain);

        assert(second.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(TestSupport::PooledObject::LiveCount.load() == 1);
        assert(pool.Acquire(
            unavailable,
            Platform::Synchronization::WaitTimeout::NoWait(),
            30
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(unavailable->Value() == 30);

        assert(first.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(unavailable.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(TestSupport::PooledObject::LiveCount.load() == 0);
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::Succeeded);
        assert(!runtime.IsInitialized());

        assert(runtime.Initialize(failure) == Memory::MemoryTopologyInitializationResult::Succeeded);
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::Succeeded);
    }


    /// Validates SharedOverflow<0> as shared-eligible without a per-Type live-instance quota.
    void TestUncappedSharedOverflow() {
        Resource resource;
        TestSupport::TestMutexProvider mutex;
        MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );
        Memory::MemoryTopologyInitializationFailure failure;

        assert(runtime.Initialize(failure) == Memory::MemoryTopologyInitializationResult::Succeeded);

        auto& pool = runtime.ObjectPoolFor<TestSupport::UncappedSharedObject>();
        Runtime::ObjectPoolType<TestSupport::UncappedSharedObject>::LeaseType first;
        Runtime::ObjectPoolType<TestSupport::UncappedSharedObject>::LeaseType second;
        Runtime::ObjectPoolType<TestSupport::UncappedSharedObject>::LeaseType third;

        assert(pool.Acquire(
            first,
            Platform::Synchronization::WaitTimeout::NoWait(),
            1
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(pool.Acquire(
            second,
            Platform::Synchronization::WaitTimeout::NoWait(),
            2
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(pool.Acquire(
            third,
            Platform::Synchronization::WaitTimeout::NoWait(),
            3
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);

        assert(first->Value() == 1);
        assert(second->Value() == 2);
        assert(third->Value() == 3);

        assert(first.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(second.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(third.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::Succeeded);
    }


    /// Validates finite/forever waiter handoff and cancellation without busy waiting.
    void TestWaitingAndCancellation() {
        Resource resource;
        TestSupport::TestMutexProvider mutex;
        MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );
        Memory::MemoryTopologyInitializationFailure failure;
        assert(runtime.Initialize(failure) == Memory::MemoryTopologyInitializationResult::Succeeded);

        auto& pool = runtime.ObjectPoolFor<TestSupport::DedicatedOnlyObject>();
        Runtime::ObjectPoolType<TestSupport::DedicatedOnlyObject>::LeaseType owner;
        assert(pool.Acquire(
            owner,
            Platform::Synchronization::WaitTimeout::NoWait(),
            1
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);

        std::atomic<Memory::ObjectPoolAcquisitionResult> waiterResult{
            Memory::ObjectPoolAcquisitionResult::CapacityUnavailable
        };
        std::atomic<int> waiterValue{0};

        std::thread waiter(
            [&]() noexcept {
                Runtime::ObjectPoolType<TestSupport::DedicatedOnlyObject>::LeaseType lease;
                const auto result = pool.Acquire(
                    lease,
                    Platform::Synchronization::WaitTimeout::ForNanoseconds(1000000000ULL),
                    2
                );
                waiterResult.store(
                    result,
                    std::memory_order_relaxed
                );

                if (result == Memory::ObjectPoolAcquisitionResult::Succeeded) {
                    waiterValue.store(
                        lease->Value(),
                        std::memory_order_relaxed
                    );
                }
            }
        );

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        assert(owner.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        waiter.join();
        assert(waiterResult.load(std::memory_order_relaxed) == Memory::ObjectPoolAcquisitionResult::Succeeded);
        assert(waiterValue.load(std::memory_order_relaxed) == 2);

        assert(pool.Acquire(
            owner,
            Platform::Synchronization::WaitTimeout::NoWait(),
            3
        ) == Memory::ObjectPoolAcquisitionResult::Succeeded);

        waiterResult.store(
            Memory::ObjectPoolAcquisitionResult::CapacityUnavailable,
            std::memory_order_relaxed
        );

        std::thread cancelledWaiter(
            [&]() noexcept {
                Runtime::ObjectPoolType<TestSupport::DedicatedOnlyObject>::LeaseType lease;
                waiterResult.store(
                    pool.Acquire(
                        lease,
                        Platform::Synchronization::WaitTimeout::Forever(),
                        4
                    ),
                    std::memory_order_relaxed
                );
            }
        );

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        assert(
            runtime.CancelPendingAcquisitions() ==
            Memory::PendingAcquisitionCancellationResult::Cancelled
        );
        cancelledWaiter.join();
        assert(waiterResult.load(std::memory_order_relaxed) == Memory::ObjectPoolAcquisitionResult::TopologyUnavailable);
        assert(owner.Release() == Memory::ObjectPoolLeaseReleaseResult::Released);
        assert(runtime.TearDown() == Memory::MemoryTopologyTeardownResult::Succeeded);
    }


    /// Validates all-or-nothing initialization and transient failing-pool context.
    void TestInitializationRollback() {
        Resource resource;
        resource.FailAllocationAt(2U);
        TestSupport::TestMutexProvider mutex;
        MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );
        Memory::MemoryTopologyInitializationFailure failure;

        const auto result = runtime.Initialize(failure);
        assert(result == Memory::MemoryTopologyInitializationResult::DedicatedReservationFailed);
        assert(failure.ObjectPoolIndex == 1U);
        assert(failure.ResourceResult == Memory::MemoryAllocationResult::ResourceExhausted);
        assert(runtime.State() == Memory::MemoryTopologyState::Uninitialized);
    }


    /// Validates that failed rollback is surfaced as a fatal initialization lifecycle state.
    void TestInitializationRollbackFailure() {
        Resource resource;
        resource.FailAllocationAt(2U);
        resource.FailRelease(true);
        TestSupport::TestMutexProvider mutex;
        MemoryComposition::Select<Memory::SharedReserveAllocationRequirement, Framework::SelectUnique> allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );
        Memory::MemoryTopologyInitializationFailure failure;

        const auto result = runtime.Initialize(failure);
        assert(result == Memory::MemoryTopologyInitializationResult::RollbackFailed);
        assert(runtime.State() == Memory::MemoryTopologyState::InitializationFailed);
    }

} // anonymous

/// Runs the complete EDP-Memory host validation suite.
int main() {
    TestSharedAllocator();
    TestObjectPoolLifecycle();
    TestUncappedSharedOverflow();
    TestWaitingAndCancellation();
    TestInitializationRollback();
    TestInitializationRollbackFailure();

    return 0;
}
