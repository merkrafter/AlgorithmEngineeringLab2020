# What is the collapse clause in OpenMP good for?
For loop parallelization with `pragma omp parallel for` by default only affects one loop in a nested loop scenario (the one that directly follows the clause).
If it has only a few number of iterations, the performance gain can be low.
Also, this behavior reduces the power of the `schedule(dynamic)` clause as the workload for each thread is much higher if it has to compute whole inner loops.
Therefore, one can linearize the nested loop with the `collapse` clause which mitigates these problems.

# Coding assignment
## Parallelize the serial pi program from the first lecture by adding only a single line.
Already done in week 1.
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

## Program 20 computes and saves an image of the mandelbrot set. Reduce the runtime of the program without degrading the image quality.
The initial runtime on my machine was:
```bash
$ make mandelbrot
$ ./mandelbrot
51.741 seconds
```

There are two places where parallelization can be placed naturally: one is the `compute_pixel` function and the other one is the nested loop in the `main` function.
The latter is by far the better choice as there are some problems with the former, including:
the function is called very often in quick succession which generates (possible) overhead for creating and tearing down threads; cancellation is needed for the function to work properly which imposes problems for the program's usability; the loop may only have a very small number of iterations, making parallel execution kind of pointless.
Last but not least, this loop has a lot of shared variables that must be synchronized in some way.
Hence, this is the optimization I went for:
```cpp
    #pragma omp parallel for default(none) shared(image) ordered schedule(dynamic) collapse(2)
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        const auto value = compute_pixel(j, i);
        #pragma omp ordered
        image << value << "\n"; // write pixel value
      }
```
The nested for loop is collapsed to maximize the dynamic scheduling's effect.
`ordered` is, of course, necessary here so that pixel values are written to the coordinates they belong to.
For this to work the computation must be pulled out of that ordered part or else the parallelization is eliminated.

In the end, this reduces the required runtime to:
```bash
$ make mandelbrot
$ ./mandelbrot
25.4512 seconds
```
