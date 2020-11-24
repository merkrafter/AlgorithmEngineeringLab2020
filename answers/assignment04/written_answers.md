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
