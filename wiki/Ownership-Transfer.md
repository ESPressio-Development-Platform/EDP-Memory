# Ownership Transfer

EDP-Memory is the authoritative domain for typed ownership/lifetime mechanics.

Use `ObjectLifetime::Construct`, `MoveConstruct`, and `Destroy` when establishing or ending an object lifetime in caller-supplied storage. Use `OwnershipTransfer::Move` when an already-typed API requires an explicit transfer expression.

Neither abstraction allocates. Downstream domains should not substitute byte copying for C++ object lifetime or directly encode their own move/lifetime policy at Memory-domain boundaries.
