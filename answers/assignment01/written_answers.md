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
