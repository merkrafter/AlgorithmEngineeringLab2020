# Explain how divide and conquer algorithms can be parallelized with tasks in OpenMP.
```
In computer science, divide and conquer is an algorithm design paradigm based on multi-branched recursion. A divide-and-conquer algorithm works by recursively breaking down a problem into two or more sub-problems of the same or related type, until these become simple enough to be solved directly. The solutions to the sub-problems are then combined to give a solution to the original problem.
```
(Wikipedia)
As the definition states, divide and conquer algorithms typically involve recursion with small(er) work packages.
A small piece of work can be modeled with OpenMPs tasks.
Instead of being executed immediately, they are enqueued and worked on as soon as a thread requests a piece of work.

# Describe some ways to speed up merge sort.
In essence, merge sort looks like this:
```
merge_sort
INPUT: (possibly unsorted) list
OUTPUT: sorted list
if length of list <= 1 return it
sorted left sublist := merge_sort left sublist
sorted right sublist := merge_sort right sublist
sorted list := merge sorted left sublist with sorted right sublist
```
As can be seen, merge sort is a divide and conquer sorting algorithm, because it calls itself with two smaller sublists until it reaches trivially sortable (1 element long) sublists.
For that reason, it can be sped up with OpenMP tasks on the recursive merge sort calls in the first place.

But there is more.
Calling functions costs time and space on the stack.
The share of this function overhead in the total sorting runtime increases with smaller sublists.
This means it is unnecessarily inefficient to sort small lists with merge sort, and lists that are shorter than a certain bound should be sorted with some other sorting algorithm.

# What is the idea behind multithreaded merging?
Merging plays a critical role in the merge sort algorithm.
It is the process of taking two sorted lists as input and producing a single sorted list containing all elements from both input lists.
A single threaded version would typically store a pointer to the smallest element in each of the input lists.
It then compares both, inserts the smaller of them into the output list and finally removes the element from its input list until both input lists are empty.

An idea to parallelize this is using a divide and conquer approach.
The input lists can be split at a specified index, for instance the index of the median of one of the lists.
This median element is then copied over to the output list to its correct position.
Then one task merges the lower parts of both lists before the median element in the output list while another task merges the greater parts behind the median.
The recursion can be transferred to iteration once the input lists are small enough, as described in the merge sort section of this document.

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

Eventually, with `-O3` specified, the following result an be obtained:
```
tasked: 8.16683 seconds (15.2315 seconds with a single thread)
std::sort: 11.3915 seconds
```

Now this is compared to Rust again.
Therefore, an initial implementation is provided that can be found in `merge_sort.rs`.
It has a runtime similar to the single-threaded, optimized implementation above.
```
naive: 15.276 seconds
Vec::sort: 10.193 seconds
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
