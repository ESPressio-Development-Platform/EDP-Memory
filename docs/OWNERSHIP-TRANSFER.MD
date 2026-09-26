# Ownership Transfer

`OwnershipTransfer::Move` is the EDP-Memory semantic boundary for explicitly transferring a typed object's ownership into another owner. It is distinct from `ObjectLifetime::MoveConstruct`: the former marks transfer intent for an existing typed expression, while the latter establishes a new object lifetime in caller-supplied storage.

Consumers should prefer `ObjectLifetime::MoveConstruct` whenever raw storage is involved. `OwnershipTransfer::Move` exists for typed APIs and standard-library containers whose constructors require an rvalue expression, preventing downstream domains from directly depending on `std::move` as their ownership policy.

Neither operation allocates memory.
