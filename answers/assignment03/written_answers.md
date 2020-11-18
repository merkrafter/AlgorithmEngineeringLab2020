# How does the ordered clause in OpenMP work in conjunction with a parallel for loop?
This construct can be used to force OpenMP run the for loop in the same ordering it would have in a single-threaded environment.
To use it, two steps are required:
First, specify `ordered` in the `#pragma omp for` clause at beginning, indicating that there is some ordered region inside the for loop.
Second, mark that region with `#pragma omp ordered`.
Note that it can hurt performance if this construct is not used carefully.

# What is the collapse clause in OpenMP good for?
For loop parallelization with `pragma omp parallel for` by default only affects one loop in a nested loop scenario (the one that directly follows the clause).
If it has only a few number of iterations, the performance gain can be low.
Also, this behavior reduces the power of the `schedule(dynamic)` clause as the workload for each thread is much higher if it has to compute whole inner loops.
Therefore, one can linearize the nested loop with the `collapse` clause which mitigates these problems.

# Explain how reductions work internally in OpenMP.
The syntax of a reduction is `reduction(op:list)` where `op` is one of `+, -, *, min, max, &, |, ^, &&`, and `||`, that is commutative, associative operators (`-` computes `-x-y` for two values `x` and `y`).
List may contain any number of variable names that should be reduced in the parallel loop.
OpenMP then keeps a local copy of the variables in the reduction list to avoid synchronization overhead and applies the reduction operation in the end, updating the global variable.
The listed local variables are initialized to an operator-specific neutral value (as 0 for +, 1 for * etc.).

If necessary, one can create an own reduction operator using this syntax:
```C
#pragma omp declare reduction
    ( identifier : typelist : combiner )
    [initializer(initializer-expression)]
```

Note: It is not recommended to use different operations in the loop body and the `reduction` clause as the effects are hard to reason about.
For instance, see the following program:
```cpp
std::vector<int> numbers{...};
int sum = 1;
#pragma omp parallel for reduction(+:sum)
for(int i = 0; i < num_numbers; i++) {
  sum *= numbers[i];
}
std::cout << sum << std::endl;
```
It always outputs 1 as the reduction variable `sum` is initialized to the + operator's initializer value 0 (rather than the specified value 1), eliminating all products in the loop, and in the end summing all 0s with the global value 1.

Further reading: [utexas.edu](https://pages.tacc.utexas.edu/~eijkhout/pcse/html/omp-reduction.html)

# Clarify how the storage attributes `private` and `firstprivate` differ from each other.
The main difference is how the values specified are initialized.
`private` variables are uninitialized, local versions of the declared variables.
Hence
```cpp
int x = 10;
#pragma omp parallel private(x)
std::cout << x;
```
likely returns 0, but not 10, as the local version of x is not initialized.
This is useful if x is huge and cloning it would be costly.

In contrast, `firstprivate` initializes the local version with a copy of the specified variable, that is
```cpp
int x = 10;
#pragma omp parallel firstprivate(x)
std::cout << x;
```
will print 10.

Note: `firstprivate` should not be used for types that have a deleted copy constructor.
The [msvc compiler documentation](https://docs.microsoft.com/en-us/cpp/parallel/openmp/2-directives?view=msvc-160#27-data-environment) says under 2.7.2.2 that `firstprivate` *can not* be applied to non-copyable types while gcc 10.2.1 accepts types with a deleted copy constructor without complaining.
When using such type as a `firstprivate` variable, one at least creates a compatibility issue.
Moreover, this could lead to resource leaks or multiple frees, if some OpenMP implementation calls the destructor on the `firstprivate` type (which gcc does not do, fortunately).
Again, this is subject to implementation specifics, as some [forum threads](https://community.intel.com/t5/Intel-C-Compiler/OpenMP-firstprivate-and-C-object-destruction/td-p/773088) show.

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
