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
        ESPressio::Memory::DedicatedInstances<2U>
    >;

    using Topology = ESPressio::Memory::MemoryTopology<
        Resource,
        ESPressio::Memory::SharedReserve<0U, Resource>,
        ValuePoolSpec
    >;

    using Composition = ESPressio::Memory::MemoryComposition<Resource>;
    using Allocator = typename Composition::template ProviderFor<
        ESPressio::Memory::SharedReserveAllocationAlgorithm
    >;
    using Runtime = ESPressio::Memory::MemoryRuntime<
        Topology,
        Composition,
        Mutex,
        Signal
    >;


    /// Demonstrates teardown refusal with a live object, followed by complete teardown and reinitialization.
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
        Runtime::ObjectPoolType<Value>::LeaseType lease;

        if (
            pool.Acquire(
                lease,
                ESPressio::Platform::Synchronization::WaitTimeout::NoWait(),
                42
            ) != ESPressio::Memory::ObjectPoolAcquisitionResult::Succeeded
        ) {
            return 2;
        }

        if (lease->Get() != 42) { return 3; }

        if (
            runtime.TearDown() !=
            ESPressio::Memory::MemoryTopologyTeardownResult::LiveObjectsRemain
        ) {
            return 4;
        }

        if (
            lease.Release() !=
            ESPressio::Memory::ObjectPoolLeaseReleaseResult::Released
        ) {
            return 5;
        }

        if (
            runtime.TearDown() !=
            ESPressio::Memory::MemoryTopologyTeardownResult::Succeeded
        ) {
            return 6;
        }

        if (
            runtime.Initialize(failure) !=
            ESPressio::Memory::MemoryTopologyInitializationResult::Succeeded
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
