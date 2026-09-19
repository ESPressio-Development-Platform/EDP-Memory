# Concurrency, Waiting and Teardown

## Coordination model

One topology-wide non-recursive Platform Mutex protects all Memory runtime bookkeeping:

- dedicated occupancy;
- shared per-Type quotas;
- shared allocator metadata;
- pending waiter list;
- runtime acquisition/lifecycle coordination.

This deliberately accepts small short-duration contention in exchange for lower permanent RAM cost and elimination of multi-lock ordering deadlocks.

User constructors and destructors never execute while the topology mutex is held.

Acquisition reserves storage under the lock, releases the lock, then constructs. Release destroys first, then returns capacity/coalesces under the lock. A new object can never reuse storage until the previous destructor has completed.

## Explicit wait policy

Every Object Pool acquisition requires an explicit Platform `WaitTimeout`:

- `NoWait` — immediate attempt only;
- finite — wait at most the requested relative duration;
- `Forever` — wait indefinitely until targeted capacity handoff/cancellation.

There is no overload with a hidden blocking policy.

## Stack-resident targeted wait requests

When a caller must block, `Acquire` creates an intrusive `WaitRequest` on that caller's existing stack. The request contains the selected Platform Signal provider instance used only for that waiter.

There is therefore:

- no heap allocation for waiting;
- no permanent native Signal per Object Pool;
- no global fixed waiter array.

Release scans waiters in arrival order and grants the oldest currently satisfiable request. Capacity is reserved for that request before its Signal is notified, preventing wake-and-steal races. Strict head-of-line blocking is intentionally avoided: an older unsatisfiable large request does not prevent a later currently satisfiable request from proceeding.

At the timeout/grant boundary, a committed grant wins if reservation completed before the waiter successfully removed itself from the queue.

EDP-Memory does not depend on EDP-Clock; finite waiting uses the Platform Signal provider's relative wait contract directly.

## Cancellation for complete shutdown

`CancelPendingAcquisitions()` marks acquisition unavailable, removes pending requests, and notifies blocked callers so `Forever` waits can return during orderly shutdown.

Cancellation never destroys existing pooled objects. New acquisitions remain unavailable until the topology has been completely torn down/reinitialized.

Final teardown still requires all leases/live objects to have been released.

## Provider failures

Synchronization provider failures are surfaced where an operation can safely return them. If a targeted notification provider fails, the runtime records coordination failure rather than pretending the handoff succeeded normally.

Initialization rollback/provider-release failures and teardown provider-release failures move the lifecycle into explicit failed states; the application should treat them as fatal provider/lifecycle conditions.

## ISR boundary

General Object Pool acquisition/release is not ISR-safe in v1. It may involve mutexes, construction/destruction, allocator traversal/coalescing, or waiting.

The intended ESPressio ISR discipline is to snapshot the minimum immediate hardware state, signal/hand off that state to an ordinary worker, and return promptly. Pooled object construction belongs in worker/execution context rather than the ISR.

## Developer diagnostics

Stale-object residency monitoring, high-water marks, fragmentation histories, and right-sizing analytics are explicitly deferred developer utilities. They must be opt-in with deterministic documented overhead and must not force core EDP-Memory to depend on EDP-Clock.
