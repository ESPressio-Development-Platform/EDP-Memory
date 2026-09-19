#pragma once

#include <cstddef>
#include <cstdint>

namespace ESPressio::Memory::Detail {

    /// Internal lifecycle of one stack-resident pending Object Pool acquisition.
    enum class WaitRequestState : std::uint8_t {
        Waiting = 0,
        Granted = 1,
        Cancelled = 2,
        ProviderFailure = 3
    };


    /// Outcome from servicing pending wait requests after capacity becomes available.
    enum class WaitRequestServiceResult : std::uint8_t {
        Succeeded = 0,
        ProviderFailure = 1
    };


    /// Intrusive stack-resident wait request carrying one targeted Platform Signal instance.
    ///
    /// @tparam TSignalProvider Concrete Platform Signal provider selected by Bootstrap.
    template<class TSignalProvider>
    struct WaitRequest final {

        // Intrusive queue links.

        /// Previous request in arrival order.
        WaitRequest* Previous = nullptr;

        /// Next request in arrival order.
        WaitRequest* Next = nullptr;

        // Requested Object Pool.

        /// Zero-based ObjectPoolSpec ordinal requested by this waiter.
        std::size_t ObjectPoolIndex = 0U;

        /// Allocation token reserved for this waiter before notification.
        std::size_t Token = 0U;

        /// Current request lifecycle state.
        WaitRequestState State = WaitRequestState::Waiting;

        // Targeted notification.

        /// Signal instance used only for the lifetime of this blocked acquisition call.
        TSignalProvider Signal{};

    };

} // ESPressio::Memory::Detail
