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

# Coding assignment
## Fix race condition bug on page 13 with a `std::mutex`.
My implementation uses the `std::mutex` together with a `std::lock_guard`.
The latter takes ownership of the mutex and locks it if possible (else it blocks).
This approach has the advantage that the mutex is unlocked and released automatically as the `lock_guard` goes out of scope after the pi summation statement.

## Reduce runtime on page 16 by choosing an appropriate schedule.
Possible schedules for OpenMP are:
- *static*: approx. equal, predetermined number of iterations for each thread
- *dynamic*: divide iterations in predetermined (many, small) chunks;
  a thread requests a new chunk when it's done with the current one
- *guided*: similar to dynamic, but chunk sizes are determined dynamically at runtime
- *auto*: compiler or OpenMP runtime choose scheduling algorithm
- *runtime*: OpenMP runtime chooses scheduling algorithm; e.g. through environment variables

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
