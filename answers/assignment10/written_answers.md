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

# What can be learn from the following quote?
```
Premature optimization is the root of all evil.
```

