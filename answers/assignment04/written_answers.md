# Explain how divide and conquer algorithms can be parallelized with tasks in OpenMP.
```
In computer science, divide and conquer is an algorithm design paradigm based on multi-branched recursion. A divide-and-conquer algorithm works by recursively breaking down a problem into two or more sub-problems of the same or related type, until these become simple enough to be solved directly. The solutions to the sub-problems are then combined to give a solution to the original problem.
```
(Wikipedia)
As the definition states, divide and conquer algorithms typically involve recursion with small(er) work packages.
A small piece of work can be modeled with OpenMPs tasks.
Instead of being executed immediately, they are enqueued and worked on as soon as a thread requests a piece of work.

# Coding warmup
## Speed up merge sort on page 7.
initially:
```
naive: 37.7612 seconds
std::sort: 62.803 seconds
```

with task (regardless of the size in if)
```
tasked: 22.3237 seconds
```

with insertion sort for n < 32 (similar for 16 and 64)
```
tasked: 18.464 seconds
```

The program still contains a lot of allocations and frees.
Such overhead can be avoided by allocating a large chunk of memory in the beginning as a buffer.
One downside of this approach is that the roles of `buffer` and `arr` must be swapped at each divide step, which reduces the readability of the program.
```
tasked: 15.4638 seconds
```

## Parallelize merging on page 18 with tasks.
initially:
```
custom merge: 1.05584 seconds
__gnu_parallel::merge: 2.41423 seconds
std::merge: 3.68582 seconds
```

After implementing the suggested improvements, the following time was achieved:
```
custom merge: 0.404156 seconds
```
