# Typed Object Lifetime

`ObjectLifetime` is the EDP-Memory abstraction for beginning, transferring, and ending C++ object lifetimes in storage that is already owned/reserved by the caller.

It deliberately performs no allocation. `Construct<T>` constructs into suitably aligned supplied storage, `MoveConstruct<T>` move-constructs into supplied destination storage while leaving the source live and moved-from, and `Destroy` ends the lifetime without releasing storage.

All supported lifetime transitions are statically constrained to non-throwing construction/move/destruction. This preserves deterministic ownership transitions and prevents higher-level EDP domains from bypassing the Memory abstraction with placement construction, direct ownership-transfer `std::move`, or explicit destructor invocation.

This facility is distinct from `ByteOperations`: byte-range movement is not valid typed lifetime transfer for arbitrary non-trivial objects.

The immediate cross-domain consumer is the Command architecture, whose fixed invocation records require correct Request/Response lifetime transitions without dynamic allocation. EDP-Threading also consumes the appropriate EDP-Memory abstraction where its existing direct move operation crosses the Memory ownership boundary.
