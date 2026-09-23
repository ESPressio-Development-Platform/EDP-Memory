#include <Arduino.h>

#include <ESPressio_Memory.hpp>
#include <ESPressio_Platform_Portable.hpp>

namespace Demo {

    /// Small application object whose complete storage fits inside one Object Pool slot.
    class Value final {

        private:

            /// Demonstration payload.
            int _value;

        public:

            /// Creates one value without additional Memory allocation.
            explicit Value(
                int value
            ) noexcept :
                _value(value) {
            }

            /// Supports deterministic pooled destruction.
            ~Value() noexcept = default;

            /// Returns the demonstration payload.
            int Get() const noexcept {
                return _value;
            }

    };


    using Resource = ESPressio::Platform::Portable::Memory::MemoryResourceProvider;
    using Mutex = ESPressio::Platform::Portable::Synchronization::MutexProvider;
    using Signal = ESPressio::Platform::Portable::Synchronization::SignalProvider;

    using ValuePoolSpec = ESPressio::Memory::ObjectPoolSpec<
        Value,
        ESPressio::Memory::DedicatedInstances<1U>,
        ESPressio::Memory::SharedOverflow<1U>
    >;

    using Topology = ESPressio::Memory::MemoryTopology<
        Resource,
        ESPressio::Memory::SharedReserve<512U, Resource>,
        ValuePoolSpec
    >;

    using Composition = ESPressio::Memory::MemoryComposition<Resource>;
    using Allocator = typename Composition::template Select<
        ESPressio::Memory::SharedReserveAllocationRequirement,
        ESPressio::System::CompositionFramework::SelectUnique
    >;
    using Runtime = ESPressio::Memory::MemoryRuntime<
        Topology,
        Composition,
        Mutex,
        Signal
    >;


    /// Demonstrates one dedicated object followed by one bounded shared-overflow object.
    int Run() noexcept {
        Resource resource;
        Mutex mutex;
        Allocator allocator;
        Runtime runtime(
            mutex,
            allocator,
            resource
        );

        ESPressio::Memory::MemoryTopologyInitializationFailure failure{};

        if (
            runtime.Initialize(failure) !=
            ESPressio::Memory::MemoryTopologyInitializationResult::Succeeded
        ) {
            return 1;
        }

        auto& pool = runtime.ObjectPoolFor<Value>();
        Runtime::ObjectPoolType<Value>::LeaseType dedicatedLease;
        Runtime::ObjectPoolType<Value>::LeaseType sharedLease;
        Runtime::ObjectPoolType<Value>::LeaseType unavailableLease;

        if (
            pool.Acquire(
                dedicatedLease,
                ESPressio::Platform::Synchronization::WaitTimeout::NoWait(),
                10
            ) != ESPressio::Memory::ObjectPoolAcquisitionResult::Succeeded
        ) {
            return 2;
        }

        if (
            pool.Acquire(
                sharedLease,
                ESPressio::Platform::Synchronization::WaitTimeout::NoWait(),
                20
            ) != ESPressio::Memory::ObjectPoolAcquisitionResult::Succeeded
        ) {
            return 3;
        }

        if (
            pool.Acquire(
                unavailableLease,
                ESPressio::Platform::Synchronization::WaitTimeout::NoWait(),
                30
            ) != ESPressio::Memory::ObjectPoolAcquisitionResult::CapacityUnavailable
        ) {
            return 4;
        }

        if (dedicatedLease->Get() != 10 || sharedLease->Get() != 20) { return 5; }

        if (
            sharedLease.Release() !=
            ESPressio::Memory::ObjectPoolLeaseReleaseResult::Released
        ) {
            return 6;
        }

        if (
            dedicatedLease.Release() !=
            ESPressio::Memory::ObjectPoolLeaseReleaseResult::Released
        ) {
            return 7;
        }

        return runtime.TearDown() ==
            ESPressio::Memory::MemoryTopologyTeardownResult::Succeeded ? 0 : 8;
    }

} // Demo

void setup() {
    static_cast<void>(Demo::Run());
}

void loop() {}
