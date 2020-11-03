# Describe how parallelism differs from concurrency.
Concurrency allows the execution of multiple tasks.
Parallelism is a subtype of concurrency that allows the execution of multiple tasks at the very same point in time.

# What is fork-join parallelism?
In fork-join parallelism, there are points where a program, for instance, starts parallel work (fork) and others where the parallelism ends and a sequential part begins (join).

# Coding assignment
## Warm up
To parallelize the pi program on slide 17, I used the OMP directive `reduction`.
It can be seen in commit `28fe5dcd76e251f`.
This way, I avoid synchronization of each single access to a shared `sum` variable.
Instead, each thread now has an own `sum` variable that is summed up with the ones from other threads in the end.
Result before parallelization:
```bash
$> make pirallel
$> ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.768738 seconds
```
Result after parallelization:
```bash
$> make pirallel
$> OMP_NUM_THREADS=1 ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.764026 seconds
$> OMP_NUM_THREADS=2 ./pirallel  # ~ 0.5 time from above
pi with 100000000 steps is 3.1415926535899099 in 0.382338 seconds
$> OMP_NUM_THREADS=4 ./pirallel  # does not give any advantage on my machine
pi with 100000000 steps is 3.1415926535896825 in 0.383342 seconds
```

```bash
$> make pirallel_opt
$> OMP_NUM_THREADS=1 ./pirallel_opt # it's as fast as the dual-thread version above
pi with 100000000 steps is 3.1415926535900236 in 0.384309 seconds
$> OMP_NUM_THREADS=2 ./pirallel_opt # again, ~ 0.5 of the single-threaded time
pi with 100000000 steps is 3.1415926535898353 in 0.196232 seconds
```

# Parallel Pi estimation through Monte Carlo simulation
Here, the parallelization was introduced in commit `ccac30834f19ce`.
The diff is cluttered due to the formatter, but there are 2 essential parts:
1. Start a parallel region including the random generator creation. This prevents every thread from using the same seed.
The `counter` is, again, reduced by summation over the whole region.
2. As before, the `for` loop is parallelized.
Using these changes, the below results could be achieved:
```bash
make pirallel_mc
$> OMP_NUM_THREADS=1 ./pirallel_mc
pi: 3.14162
run_time: 17.157 s
n: 100000000
$> OMP_NUM_THREADS=2 ./pirallel_mc
pi: 3.14167
run_time: 9.04005 s
n: 100000000
```
This time, the multi-threaded version is not exactly twice as fast as the single threaded one.
A possible cause for this is that there is more to do than the parallelizable loop.
```bash
$> make pirallel_mc_opt
$> OMP_NUM_THREADS=2 ./pirallel_mc_opt
pi: 3.1415
run_time: 1.70091 s
n: 100000000
```
This time, the optimized version is about 5 times as fast as without `-Ofast`.
