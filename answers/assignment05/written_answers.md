# What is CMake?
```
CMake is an open-source, cross-platform family of tools designed to build, test and package software.
CMake is used to control the software compilation process using simple platform and compiler independent configuration files, and generate native makefiles and workspaces that can be used in the compiler environment of your choice.
```
[cmake.org](https://cmake.org)
The mentioned "configuration files" are actually much more powerful, in fact, they are Turing-complete as it supports branching and setting memory.

# What role do targets play in CMake?
Targets (libraries, executables) are the output of CMake files in the sense that they are what the user wants to build.
They can be imagined as objects that are constructed with `add_library` and `add_executable`.
Each target has a unique name and a list of files that it depends on.
Furthermore, targets have properties as compile options (e.g. static vs. shared libraries) and methods that can be called on them as `target_link_libraries`.

# How would you proceed to optimize code?
1. Write correct and clean code and back it up with tests.
   Having correct code for the task at hand is the most important thing.
   It does not matter whether your code runs fast if it does not solve the problem it was written for.
   Remember, "premature optimization is the root of all evil" is a popular saying in software engineering as is tends to lead to unreadable software
   if programmers use cryptic instructions and functions that are said to improve the performance.
2. Measure and identify the bottlenecks
   The next step is measuring the run time of the system.
   If it is sufficient for the problem to solve, one should strongly consider not to optimize further at all as it might introduce new bugs, hurt the maintainability and costs time that could be used otherwise.
   In case the performance achieved is not high enough, the bottleneck(s) must be identified.
   It makes no sense to put optimization effort in parts of the program that account for very little of the overall runtime.
3. Implement the optimization while testing continuously
   Choose an algorithm with better theoretical complexity if possible.
   When the bottleneck part of the program is compute-bound, think about caching results, vectorizing, or parallelizing code, and optimizing the memory access patterns.
   When it's bandwidth-bound, streaming stores might help out as well as the aforementioned improvement of memory accesses patterns.
   After implementing these improvements, test to see whether the code is still functionally correct and pay attention the code is still readable.
   Then go to step 2.
