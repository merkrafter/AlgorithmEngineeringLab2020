# Name and explain some useful compiler flags during development
During development, it is advisable to enable all warnings to find bugs and poor design as early as possible.
Optimization flags should generally not be used to ensure all debug assertions are executed, which otherwise would be deleted in the release candidate.
Another helpful option is to include debug symbols (function/variable names, line numbers, source code) in the created binary which helps navigating in debuggers.

How these flags are set depends on the concrete compiler, e.g. here for [GCC](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html), [clang](https://clang.llvm.org/docs/UsersManual.html#options-to-control-error-and-warning-messages).
There is, however, the tendency to use `-Wall` for linting and `-g` for including debugging symbols.
Modern programming languages as Rust are strict by default and don't really offer something like `-Wall`.
For additional linting, external tools as [Clippy](https://github.com/rust-lang/rust-clippy) can be used.

# How could Intel oneAPI help you write better programs?
From [the official page](https://software.intel.com/content/www/us/en/develop/tools/oneapi.html) "oneAPI is an open, unified programming model built on standards to simplify development and deployment of data-centric workloads across CPUs, GPUs, FPGAs and other accelerators."

The Intel implementation additionally comes with a plethora of useful tools for getting insights in the performance profile of own programs.
![Intel oneAPI base-toolkit](https://software.intel.com/content/dam/develop/public/us/en/images/diagrams-infographics/diagram-onapi-base-toolkit-16x9.png)
The VTune Profiler, can find performance bottlenecks, for instance parts that suffer from bad cache access.
The Advisor includes but is not limited to a vectorization report to improve the use of vector arithmetic in the program.

# What can be learn from the following quote?
```
Premature optimization is the root of all evil.
```
[Assignment 05](../assignment05/written_answers.md) already explains this quote in the optimization process part.
The problem here is that you shouldn't waste time with "optimizations" you'll never need, especially if it decreases the program's code quality.
First and foremost, the program should be correct.
Next, it should provide useful abstractions that allow easy modifications and extension.
Only then come optimizations, if they are even necessary.
