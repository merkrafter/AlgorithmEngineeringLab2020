# Coding assignment
## Write a parallel program for computing pi using only standard C++.
This was the initial runtime of the program:
```bash
$ make pirallel
$ ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.773447 seconds
```
My parallelization approach is using an array for storing partial results of the threads and reduce them afterwards.
It therefore is statically scheduled which is totally fine for this task.
In the end, the run time can be cut in half with two threads as expected:
```bash
$ make pirallel # with 2 threads
$ ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.383523 seconds
```
Other possible approaches include 1) utilizing a mutex that is shared between threads that write to a single result variable, 2) defining the result variable as an atomic and passing it around and 3) defining a synchronized queue as a consumer that threads write their results to that are then summed up by another thread simultaneously.
