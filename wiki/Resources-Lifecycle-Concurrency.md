# Resources, Lifecycle and Concurrency

The runtime has bounded metadata and no hidden heap fallback. One non-recursive Platform mutex protects topology bookkeeping. Blocked acquisition uses caller-stack wait records plus targeted Signal wakeups. Release grants the oldest currently satisfiable waiter and may skip an unsatisfiable head to avoid head-of-line blocking.

Every Object Pool now also exposes a strong dedicated-slot index backed by `EDP-BoundedTopology::BoundedIndex`. The locked dedicated capacity ceiling of 255 keeps this identity at exactly one byte. No additional per-live-object pool pointer or release token is retained by indexed consumers.

Indexed acquire/release uses the same topology mutex and occupancy bitmap as ordinary leases; object construction/destruction remains outside the mutex. Releasing indexed capacity participates in normal waiter service. A live indexed slot blocks teardown exactly like a live lease-backed dedicated object.

General pool operations, including indexed ownership, are not ISR-safe. Teardown refuses live objects/leases and supports explicit cancellation of pending acquisitions.
