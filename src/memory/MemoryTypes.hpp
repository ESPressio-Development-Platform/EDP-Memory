#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::Memory {

    /// Lexicographical ordering produced by raw byte comparison.
    enum class ByteComparison : std::uint8_t {
        Less = 0,
        Equal = 1,
        Greater = 2
    };


    /// Outcome from requesting one raw aligned allocation from a MemoryResource provider.
    enum class MemoryAllocationResult : std::uint8_t {
        Succeeded = 0,
        InvalidSize = 1,
        InvalidAlignment = 2,
        ResourceExhausted = 3,
        ProviderFailure = 4
    };


    /// Outcome from returning one raw allocation to its originating MemoryResource provider.
    enum class MemoryReleaseResult : std::uint8_t {
        Released = 0,
        InvalidBlock = 1,
        ProviderFailure = 2
    };


    /// Outcome from initializing a shared-reserve allocation algorithm over one fixed backing block.
    enum class SharedReserveInitializationResult : std::uint8_t {
        Succeeded = 0,
        InvalidBlock = 1,
        AlreadyInitialized = 2
    };


    /// Outcome from requesting one allocation from the bounded shared reserve.
    enum class SharedAllocationResult : std::uint8_t {
        Succeeded = 0,
        InvalidSize = 1,
        InvalidAlignment = 2,
        CapacityUnavailable = 3,
        ContiguousCapacityUnavailable = 4,
        NotInitialized = 5
    };


    /// Outcome from returning one allocation to the bounded shared reserve.
    enum class SharedAllocationReleaseResult : std::uint8_t {
        Released = 0,
        InvalidAllocation = 1,
        NotInitialized = 2
    };


    /// Outcome from tearing down one shared-reserve allocator instance.
    enum class SharedAllocatorTeardownResult : std::uint8_t {
        Succeeded = 0,
        NotInitialized = 1,
        AllocationsRemain = 2
    };


    /// Runtime lifecycle state of one configured Memory topology.
    enum class MemoryTopologyState : std::uint8_t {
        Uninitialized = 0,
        Initializing = 1,
        InitializedFrozen = 2,
        TearingDown = 3,
        InitializationFailed = 4,
        TeardownFailed = 5
    };


    /// Outcome from synchronously establishing one complete configured Memory topology.
    enum class MemoryTopologyInitializationResult : std::uint8_t {
        Succeeded = 0,
        AlreadyInitialized = 1,
        DedicatedReservationFailed = 2,
        SharedReserveReservationFailed = 3,
        AllocatorInitializationFailed = 4,
        RollbackFailed = 5,
        ProviderFailure = 6
    };


    /// Outcome from cancelling acquisition requests that are presently waiting for capacity.
    enum class PendingAcquisitionCancellationResult : std::uint8_t {
        Cancelled = 0,
        NotInitialized = 1,
        AlreadyCancelled = 2,
        ProviderFailure = 3
    };


    /// Outcome from tearing down one initialized Memory topology.
    enum class MemoryTopologyTeardownResult : std::uint8_t {
        Succeeded = 0,
        NotInitialized = 1,
        TopologyUnavailable = 2,
        PendingAcquisitionsRemain = 3,
        LiveObjectsRemain = 4,
        AllocatorTeardownFailed = 5,
        ProviderReleaseFailed = 6
    };


    /// Outcome from acquiring one live object from an ObjectPool.
    enum class ObjectPoolAcquisitionResult : std::uint8_t {
        Succeeded = 0,
        CapacityUnavailable = 1,
        TimedOut = 2,
        NotInitialized = 3,
        TopologyUnavailable = 4,
        OutputLeaseOccupied = 5,
        ProviderFailure = 6
    };


    /// Outcome from explicitly releasing one ObjectPoolLease.
    enum class ObjectPoolLeaseReleaseResult : std::uint8_t {
        Released = 0,
        AlreadyEmpty = 1,
        ProviderFailure = 2
    };


    /// Raw aligned allocation owned by the MemoryResource provider that produced it.
    struct MemoryBlock final {

        // Raw allocation description.

        /// First writable byte of the allocation, or null only for an empty descriptor.
        void* Address = nullptr;

        /// Actual usable byte count owned by this allocation.
        std::size_t Size = 0U;

        /// Alignment satisfied by Address.
        std::size_t Alignment = 0U;

    };


    /// Opaque allocation returned by a shared-reserve allocation algorithm.
    struct SharedAllocation final {

        // Allocation identity.

        /// Byte offset of the live payload from the start of the shared reserve.
        std::size_t PayloadOffset = 0U;

    };


    /// Transient diagnostic context populated when topology initialization fails.
    struct MemoryTopologyInitializationFailure final {

        // Failure context.

        /// Initialization result associated with this context.
        MemoryTopologyInitializationResult Result = MemoryTopologyInitializationResult::Succeeded;

        /// Zero-based ObjectPoolSpec ordinal, or NoObjectPoolIndex when failure is not pool-specific.
        std::size_t ObjectPoolIndex = static_cast<std::size_t>(-1);

        /// Underlying raw resource result when resource allocation caused the failure.
        MemoryAllocationResult ResourceResult = MemoryAllocationResult::Succeeded;

        /// Reserved ordinal used when no Object Pool is associated with the failure.
        static constexpr std::size_t NoObjectPoolIndex = static_cast<std::size_t>(-1);

    };

} // ESPressio::Memory
