# Architecture

The library has two layers. Abstract capabilities describe ByteOperations, MemoryResource and SharedReserveAllocationAlgorithm. The deterministic runtime layer declares a complete MemoryTopology at compile time, including dedicated typed pools and one bounded shared reserve.

Object acquisition prefers dedicated capacity, then eligible shared overflow, then optionally waits using a bounded waiter record. The topology is frozen while initialized and can only be replaced after complete teardown.
