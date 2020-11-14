# What causes false sharing?
The concept of false sharing is tightly coupled with cache lines.
A cache line is the smallest amount of memory that can travel through the system, commonly 64 byte, but probably 128 byte soon.
In particular, this means that processors can not just access that single variable they need, but have to request a whole cache line to work with.

False sharing is caused by two or more processors that need to *update* variables (not necessarily the same) that are stored on the same cache line.
The cache coherency mechanisms must then invalidate the cache line and synchronize it between the processors, resolving any possible conflicts, which is time-costly.
It is called "false" sharing because it creates management overhead for contentwise independent variables, just because the memory is laid out poorly.

In practice, false sharing should be considered, but is often not a problem as most of the time is used for computation.
However, it can become a major performance bottleneck, if a program frequently writes data to a shared cache line.
Note that this can happen quickly if some status variables are defined one after another in the program which makes it likely for them to be stored on the same cache line.

# How do mutual exclusion constructs prevent race conditons?
Race conditions are concurrent parts of the program whose result depends on the execution order.
A popular example is the following:
```cpp
int count = 0;
#pragma omp parallel for
for (int i = 0; i < SOME_NUMBER; i++) {
  count++;
}
```
The `count++` might look like an atomic operation to an untrained eye, but in reality its 3 instructions:
```asm
<+4>:     mov    eax,DWORD PTR [rip+0x2ee8] # count
<+10>:    add    eax,0x1
<+13>:    mov    DWORD PTR [rip+0x2edf],eax # also count
```
Now both threads could read `count` at roughly the same time, say both read the value 0.
They both add 1 to their `eax` register (which result in 1) that both threads write back to the address of `count`.
In the end, two additions were made but `count` is only 1.

Formally, a race condition occurs when two or more threads access a single resource (variable `count` in the example above) at the same time, if at least one thread writes to that resource.
If used correctly, mutual exclusion constructs can be used by a thread to get exclusive access to a resource.
A thread can lock a critical section, that is no other thread can execute code in that section simultaneouly, do some work and in the end unlock so that other threads can enter the critical section.
Therefore any race conditions are eliminated.
Of course it does not mitigate the problem if, say, multiple threads can read a value at the same time and only the write-back is protected.

# Explain the differences between static and dynamic schedules in OpenMP.
## Static scheduling
*Static* scheduling deals an approximately equal, predetermined number of iterations to each thread.
This may result in a sub-optimal distribution of workload if the iterations are unbalanced (see exercise below), but has little to none overhead at runtime.
By default, the compiler determines the amount of overall iterations (there are strict limitations on the types of loops that can be auto-parallelized that make it simple to calculate this number), divides it by the number of threads and assignes loops accordingly.
The simplicity of this approach can be seen with the following setup:
```cpp
#pragma omp parallel
{
  const int thread_id = omp_get_thread_num();
  const int num_threads = omp_get_num_threads();
// WARNING: faulty distribution of work
#pragma omp for schedule(static)
  for(int i = thread_id; i < 1000; i+= number_threads) {
    if(thread_id==2) {
      std::cout << i << std::endl;
    }
  }
}
```
It prints the numbers 506, 510, 514, ..., 750.
Here, one must be really careful, because the code is correct for a single-threaded execution, but can skip an arbitrary number of iterations for multiple threads.
Takeaway: do not try to schedule code yourself when also using the automatic scheduling the the `for` clause.

Static scheduling can take an argument, n, that defines the size of the chunks of iterations that each thread gets assigned.
It can be seen here:
```cpp
#pragma omp parallel for schedule(static,1)
for(int i = 0; i < 1000; i++) {...}
```
which gcc 10.2.1 compiles down to
```asm
<+17>:    call   <omp_get_num_threads@plt>
<+22>:    mov    r12d,eax                   # e.g. 4
<+25>:    call   <omp_get_thread_num@plt>
<+30>:    mov    r13d,eax                   # e.g. 2
<+33>:    mov    ebx,0x0                    # loop iteration count
<+38>:    mov    eax,ebx                    # i = ebx (0 initially)
<+40>:    imul   eax,r12d                   # i *= num_threads
<+44>:    add    eax,r13d                   # i += thread_id
...
<+105>:   add    ebx,0x1
<+108>:   jmp    <+38>
```
This lets the loop start at `0*num_threads + thread_id`, continue with `1*num_threads + thread_id` etc.
Notice that two registers are involved in controlling `i`'s value: `eax`, but also, and more importantly, `ebx`.
The implications of this approach of updating `i` will be in focus later on again.

If there are more iterations than `n*num_threads`, the allocation starts over.
For 3 threads, 12 (`0-0xb`) iterations and `n=1`, the following assignment will be created:
thread 0: 0 3 6 9
thread 1: 1 4 7 a
thread 2: 2 5 8 b

Static scheduling works best if all iterations have an approximately equal workload.
It can even be used when the programmer knows the distribution of workload, e.g. whether it increases monotonically with increasing loop counter.
Then `schedule(static,1)` implements a somewhat equal share of work for each thread.

## Dynamic scheduling
*Dynamic* scheduling also divides the iterations into chunks, but assignes them at runtime to the threads.
That is, if a thread becomes free, it requests the next chunk of iterations from the OpenMP runtime.
This model come with some management overhead, but is better suited than static scheduling for loops with unbalanced workload in each iteration, espcially if nothing is known about the distribution of workload across the iterations.

## More scheduling
Apart from static and dynamic, OpenMP allows the following schedule modes:
- *guided*: similar to dynamic, but chunk sizes are determined dynamically at runtime
- *auto*: compiler or OpenMP runtime choose scheduling algorithm
- *runtime*: OpenMP runtime chooses scheduling algorithm; e.g. through environment variables

# What can we do if we've found a solution while running a parallel loop in OpenMP, but still have many iterations left?
The goal here is to stop the loop after a solution has been found to avoid running all the next loop iterations and wasting time.

1. The first approach is using a shared flag that threads can query and overwrite with `true` when a solution is found.
In theory, the flag could be defined as `bool is_done(false);` since it can only ever be turned on.
If two threads attempt to write `true` to it at the very same time, it does not make a difference: the flag is on afterwards anyway.
If a thread reads the flag as `false` while another thread is in the process of writing `true` to it, it does not hurt much either as the first thread will only do another loop iteration.
However, then the compiler comes into play.
If the flag is defined as a simple `bool`, the compiler is not aware the variable is shared and might optimize it away completely (this is unlikely, though, as it is modified conditionally).
Fortunately, there is a "zero cost" solution for this special case: `std::atomic<bool>`.
As [Leis](https://databasearchitects.blogspot.com/2020/10/c-concurrency-model-on-x86-for-dummies.html) points out, atomic reads are as fast as normal ones.
The same is actually almost true for writes since the memory order can be relaxed.
Setting the flag to `true` directly invokes the method `atomic::store` with the expensive argument `std::memory_order_seq_cst` which flushes the change into the caches shared by all CPUs to propagate it directly.
A far better result can be achieved when calling `store` and passing `std::memory_order_release`.
This allows to defer the propagation of the change, which is totally acceptable for this flag.
It does not matter if user threads do two or three more iterations if the whole construct can save them thousands or even millions.
In the end, there is only a single write anyway, so false sharing is not a big deal here as well.
What remains is the question what should be done if the flag is true as `break` is not allowed here.
The first thing that could come to ones mind is the following line of thoughts.
A `for` loop stops, when its end condition is met.
OpenMP only allows loops whose condition is of the form `iteration_variable < constant_upper_value` (or lower) so inserting the flag itself is prohibited.
This would not be a problem as the condition can be fulfilled anyway: just check the flag in the loop and if its `true`, set the iteration variable (let's name it `i`) to the upper value.
At the end of the current iteration, the loop checks its condition, which is `false`, and stops. Right?
Actually, no.
Remember the section about OpenMP schedules where it could be seen how exactly the iteration variable is updated.
The code counts the loop iteration number and calculates `i` from that instead of maintaining `i` directly.
This means that however `i` is modified during the loop, the change is discarded at the end of the loop.

The probably second-best solution here is to check the flag at the beginning of the loop and `continue` if it's set.
This effectively fast-forwards the loop in a branch prediction-friendly way and hence is quite efficient.

2. To mitigate the missing `break`, OpenMP introduced the concept of cancellation points.
It pretty much automates the creation of the above-mentioned flag.
To use this mechanism, the programmer can insert a `#pragma omp cancel for` in place of the `break` to set the internal flag.
Whenever a thread reaches an OpenMP barrier, it checks the flag and stops the loop if appropriate.
Additionally, the programmer can specify `#pragma omp cancellation point for`s that force a check of the flag.
OpenMP's cancellation feature is somewhat expensive, hence it is disabled by default.
To activate is, the environment variable `OMP_CANCELLATION` must be set to `true`.
More info at: http://jakascorner.com/blog/2016/08/omp-cancel.html

# Coding assignment
## Fix race condition bug on page 13 with a `std::mutex`.
My implementation uses the `std::mutex` together with a `std::lock_guard`.
The latter takes ownership of the mutex and locks it if possible (else it blocks).
This approach has the advantage that the mutex is unlocked and released automatically as the `lock_guard` goes out of scope after the pi summation statement.

## Reduce runtime on page 16 by choosing an appropriate schedule.
As the file name indicates, the workload is unbalanced.
Therefore, the static schedule is not the best option here as the iterations on one end of the for loop are much more computationally intense than on the other end.
This means that the first thread will be done quickly while all threads will have to wait for the last one in the end.
The following measured times show this property:
```
static 2.46052 seconds
dynamic: 1.25791 seconds
guided: 2.44866 seconds # I suppose the runtime overhead is too large
auto: 2.43807 seconds # probably chooses static or guided
runtime: 1.22664 seconds
```

Based on that, if the task is to choose a schedule option here, it would be `dynamic` or `runtime`.
But that is not the whole picture.
If the workload is unbalanced, but its characteristics are known, we can exploit that.
In this example, as already mentioned, the workload of the images on one end of the vector is known to be much higher than on the other end.
It is therefore not the best idea to give one end to one thread and the other end to another one.
A better option would be passing to one thread images 0, 2, 4, ... and the ones with odd numbers to another (thread 2 would still have a higher workload, but it's a great improvement over the first approach).
This can be achieved by using the schedule(static, 1), which assigns loop iterations to threads in Round Robin fashion.
It yields comparable results to the dynamic scheduling:
```
static, 1: 1.25501 seconds
```
It does not have the overhead at runtime as dynamic scheduling, but probably fails to schedule the last iterations as efficiently as the dynamic configuration.

## What schedule on page 18 does produce the following result?
```
thread 3 computed the iterations:
thread 0 computed the iterations: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
thread 1 computed the iterations: 22 23 24 25 26 27 28 29 30 31
thread 2 computed the iterations:
```
One can see that 2 of 4 thread did not get any workload, hence there is probably static scheduling involved.
As the first thread (number 0) gets 22 items while the second only gets 10, and the later get nothing, this is no automatic static scheduling, but user specified.
In the end, this result can be "achieved" by specifying `schedule(static, 22)`.

## Modify the program on page 21 so that it always finds the smallest possible solution. How does `std::atomic::compare_exchange_weak` work?
A trivial approach to this problem would be to remove the `solution_found` flag and the `continue` altogether and update `final_solution` with smaller values in a critical section.
This, however, would not exploit our knowledge of the problem.
First, as the objective value is `i` itself (instead of a complicated function of `i`) we can `continue` each iteration with an `i` value larger than `final_solution` and thus skipping most of the iterations.
The tricky part left is the update of `final_solution`, where `std::atomic::compare_exchange_weak` comes into play.
[Its documentation](https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange) states that it gets two arguments, a reference `expected` and a value `desired`.
It then compares `expected` with the internal value of the atomic and if they're equal, the internal value is replaced with `desired`.
If they are different, `expected` is replaced with the internal value so that the caller knows the internal value and does not have to read it by itself a second time.
The return value of this method is `true` if the internal value changed.
As the "weak" in this method's name indicates, it does not always work properly.
Instead, it is allowed to leave the internal value untouched and update `expected` even if `expected` is equal to the internal value.
Thus, the call should be wrapped with a while loop.
In our example code we can use the method to update the atomic variable with smaller `i` values.
```cpp
while(previous > i && !final_solution.compare_exchange_weak(previous,i)) {}
```
translates logically to
```cpp
if(final_solution > i) {
  final_solution = i;
}
```
but in a thread-safe, atomic, and lock-free way.
