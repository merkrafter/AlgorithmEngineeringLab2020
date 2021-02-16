# How do bandwidth-bound computations differ from compute-bound computations?
Both terms are named after the bottleneck resource.

If the CPU has a high workload, the computation is compute-bound.
This is the desired case, as the CPU does as much useful work as possible.
When the program does not meet the performance requirements, another algorithm must be chosen, better hardware acquired, or techniques from the previous assignments utilized.

If the CPU does the computations way faster than the memory can deliver data, the computation is bandwidth- (or memory-)bound.
This can be seen in a profiler, if a large portion of the runtime is used by memory access.
Avoiding such algorithms is important to not waste resources.
In this assignment, solutions for this case are presented.

# Explain why temporal locality and spatial locality can improve program performance.
Temporal locality is present if the time interval between to accesses to the same memory location in a program is short.
In that case, the program can use the CPU's fastest memory, that is one of the caches or even registers.

Spatial locality means that the distance between the addresses of consecutively accesses variables in a program is low.
This way the program can benefit from the way the caches work:
the smallest amount of data that can be transferred between the memory components of a CPU is a cache line (typically 64 bytes).
Even if the program only accesses a single byte, 63 more of them are fetched and loaded into the cache.
Then, when the next variable is needed, it is beneficial if it is included in the previously loaded cache line.


# What are the differences between data-oriented design and object-oriented design?


# What are streaming stores?
Streaming stores are a technique to circumvent caches by reading from and writing to the RAM directly.
While it is usually desirable to use the cache hierarchy, it is designed to reduce the access time to frequently used variables.
Some applications, however, will load and store the same data only once (think of inner product computation).
In these scenarios, using stream stores avoids pollution of the cache to keep it available for actually cache-worthy data.

# Describe a typical cache hierarchy used in Intel CPUs.
The typical cache hierarchy consists of several tiers.
From top to bottom, they get slower, larger and cheaper, and are usually placed further away from the CPU.

The fastest memory available are registers.
There are many types of registers for various tasks, but of major interest for this consideration are general-purpose registers and vector registers, as they allow for arbitrary computation.
Registers are attached to the CPU itself which is why they are listed here besides not being part of the cache hierarchy itself.

The next tiers are formed by the actual cache hierarchy in the narrower sense.
Usually, a CPU has 3 cache levels, named L1 to L3 cache.
L1 is the smallest (few KiB) and fastest of these and devided into disjoint stores for data and instructions so that neither of them blocks the other.
L2 caches store a few hundreds of KiB of both instructions and data.
Together with the L1 caches, L2 caches are replicated on each CPU core.
This is beneficial if the cores work on different memory regions as they don't interfere with each other.
On the downside, this architecture requires cache coherence mechanisms that hurt performance especially in the presence of false sharing (see [assignment 2](../assignment02/written_answers.md)).
Finally, there is a L3 cache that contains few MiB and is shared between all cores.

# What are cache conflicts?
As caches only have a certain amount of memory to work with, they must eventually evict (preferably old) cache lines when new ones arrive while the cache is full.
In practice, caches are implemented as hash tables with buckets of size N.
This means a cache can take up to N elements with the same hash which, in turn, depends on the memory address of the cache line.
Cache conflicts occur when a program accesses data in such a pattern that their addresses produce the same hash frequently.
Consequently, only a small fraction of the cache can be used effectively which can result in a huge performance drop.
