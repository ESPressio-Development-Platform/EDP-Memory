# G1 #16 — independently reproduced outstanding native Memory waiter defects

These fixtures are **negative reproductions of known unsafe behaviour**, not ordinary
passing production tests. They are intentionally excluded from `tests/run_host_tests.sh`
until the full native finite-wait/teardown redesign has been implemented and the
fixtures have been converted to normal positive regressions.

## Cancelled waiter teardown reproducer

File: `g1_cancelled_teardown_reproducer.cpp`.

A finite-capacity pool is exhausted while a second caller waits. The test Signal
provider deliberately holds the second caller in its `Wait` call after cancellation,
so it is detached from the intrusive queue but remains active in `AcquireObject`.
The original runtime checks only `_waitHead` and incorrectly allows `TearDown()`
to return `Succeeded`. The fixture expects the strongly typed result
`PendingAcquisitionsRemain` until the caller returns, and exits **43** on the
known unsafe `Succeeded` result. It then resumes and joins the caller safely.

Observed on RPI400 2026-10-16 with `EDP-Memory/feature/logging-v1`
`3326426c5c63e99884aa1787e35d727519ed1793`:
`G1 cancelled waiter: teardown=0 pending=0`, exit **43**,
detached runner `20261016T150919-dd8a287f`.

For a manual focused run (from a workspace with sibling EDP dependencies):

```sh
g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -pthread \
  -I EDP-Memory/src -I EDP-System/src -I EDP-Platform/src \
  -I EDP-BoundedTopology/src \
  EDP-Memory/tests/known_failures/g1_cancelled_teardown_reproducer.cpp \
  -o /tmp/g1_cancelled_teardown_repro
/tmp/g1_cancelled_teardown_repro
```

**Do not treat this controlled negative result as an accepted solution.**
The native pending acquisition still carries caller-stack Signal/list ownership;
other failure cases can cause use-after-return, reserved-token leaks, unsynchronised
lifecycle reads and indefinite cleanup relocking. See EDP-Memory#16 and the
authoritative Logging handoff for the required owner-level redesign.
