# Explain the naming conventions for intrinsic functions
Intrinsic functions usually follow the naming scheme `_<vector_size>_<function>_<data_type>`
where
- `<vector_size>` specifies the size of the processed vectors.
  `m` stands for 64 bit, `mm` for 128 bit, `mm256` for 256 bit and `mm512` for 512 bit
- `<function>` specifies the kind of computation that the instrinsic is translated into.
  Examples are `abs`, `reduce_add` or `sha256rnds2`.
- `<data_type>` specifies the data type and number format of the intrinsic.
  `ps` (packed single) uses single-precision floating-point values,
  `pd` (packed double) uses double-precision floating-point values,
  `ep<format>` uses integers, where `<format>` uses `u` and `i` for signedness and a number indicating the bit length of the integers.
  Instead of `p`, the data type may use a `s`.
  In that case, the intrinsic operates on single values, hence it is no *actual* vector operation in the narrower sense.

As an overall example, `_mm512_add_epi8` adds signed 8-bit integers, operating on 512/8 = 64 of them at the same time.

Note that `<vector_size>` is not necessarily the size of the returned value (e.g. for reduction instructions).

exceptions: `_mm_avg_pu16` -- works on 64 bit vectors that represent packed u16 values

# What do the metrics latency and throughput tell you about the performance of an intrinsic function?
Latency denotes the number of cycles the hardware needs to get the result of an operation.
Throughput denotes how many cycles are necessary before the next operation of the same kind can be started.
Both concepts are related, and both influence the overall performance of an algorithm, but they are not quite the same.
Given a super complex computation has a latency of 10, it takes super long before the result is available, indicating that running this computation over a lot of data is not feasible.
If, however, the throughput is high, for instance allowing 2 computations to be started per cycle, the picture looks much different.
After a ramp-up of 10 cycles, two results will be available at each cycle.

# How do modern processors realize instruction-level parallelism?
Modern processors have multiple functional units, identified by a so called port.
For instance, there may be 2 (or more) ports that have a vector ALU in the same CPU core.
This enables CPUs to execute multiple instructions on the same core.
In contrast, SIMD enables the execution of the same operation on multiple data in a single instruction.
Both concepts can be combined, that is a CPU may execute two SIMD operations on two vector ALUs simultaneously.

# How may loop unrolling affect the execution time of compiled code?
Loop unrolling is a technique where multiple instances of the loop body are executed in the same iteration, leading to fewer iterations.
For instance, compare the computation of the inner product in [unrolling.cpp](./unrolling.cpp):
Here, a loop is unrolled by a factor of 4.
This avoids ~ 3/4 of all branch checks at the loop head, and exploits instruction level-parallelism better, because the same instructions are used directly after another without the branching logic in between.
The effect is a speedup by a factor of 2 in this case:
```bash
$ gcc -fopenmp -O3 unrolling.cpp
$ ./a.out
normal loop: 0.193157 seconds
unrolled loop: 0.103007 seconds
```
Looking at the assembly, the unrolled version was able to use vector instructions, although the (un-)packing and reduction need some additional instructions.

Loop unrolling is no silver bullet, however.
First, it enlarges the code, possibly making it harder to understand, and more difficult for a compiler to process and optimize.

# What does a high IPC value mean in terms of the performance of an algorithm?
There are several performance metrics for algorithms.
Among these, IPC (Instructions per cycle) have the advantage that they are mostly agnostic to the clock speed of the processor the algorithm is running on.
This makes IPC a great measure for the algorithm itself.
