# Resources, Lifecycle and Concurrency

The runtime has bounded metadata and no hidden heap fallback. One non-recursive Platform mutex protects topology bookkeeping. Blocked acquisition uses caller-stack wait records plus targeted Signal wakeups. Release grants the oldest currently satisfiable waiter and may skip an unsatisfiable head to avoid head-of-line blocking.

General pool operations are not ISR-safe. Teardown refuses live objects/leases and supports explicit cancellation of pending acquisitions.
