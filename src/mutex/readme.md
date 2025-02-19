Windows-specific mutex implementation, for Linux I'd use futex and native clock_gettime for benchmarking cpu time
TODO: Dynamic mutex, that naively stays on the CPU for some time
