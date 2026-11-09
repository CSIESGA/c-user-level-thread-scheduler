# User-Level Thread Scheduler in C

A small user-level threading runtime that runs multiple tasks inside one Unix process. The scheduler switches execution contexts with `sigsetjmp()` and `siglongjmp()` and uses Unix signals to trigger scheduling. Threads can yield, exit, or wait for another thread and collect its return value.

## Build and run

From the repository root on Linux with GCC and Make installed:

```sh
make
./main 3 6 1 4 -1 -1
```

This example runs Fibonacci and Stirling number tasks. See `sample_execution.sh` for examples of signal handling and thread joins.
