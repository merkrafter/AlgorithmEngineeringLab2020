# What is Cython?
From [their website](https://cython.org/):
```
Cython is an optimising static compiler for both the Python programming language and the extended Cython programming language.
```
It is a superset of Python that also allows specifying types for variables and functions.
Type checks can now be carried out at compile time, accelerating programs at run time.

# Describe an approach how Python programs can be accelerated with the help of Cython.
This directory contains an example project.
One thing that is particularly slow in Python is loop execution.
For instance, let's assume we want to speed up the `python_is_prime` function in [main.py](./main.py), because it was identified as the bottleneck of the application.
Without any judgement, the baseline performance for the input 2305843009219 is 0.124s.
The first step now is to create a `.pyx` file to place Cython code in.
This can be imported into main.py as any other Python module (is has to be compile before being used, though. See next section).
Next, one can simply copy the Python code into the Cython file and already achieve a small speedup: 0.092s
(well, almost. The C `sqrt` function must be imported.)
Now, C types can be added to the variables `z`, `i` and `upper`.
This boosts the performance by an additional factor of ~10.

Overall:
```sh
$ python main.py 2305843009219
Python True 0.12401962280273438s
Cython True 0.007615327835083008s
```

# Describe two ways for compiling a .pyx Cython module.
For development and testing, the `cythonize` command can be used to generate an extension module directly from Cython (`.pyx`) files.
It wraps the process of translating the Cython file into a C file which is then compiled by a C compiler into the extension module.

The other option is suitable for packaging and distributing software with Cython parts: setuptools.
This approach requires to create the files `setup.py` and `pyproject.toml`.
An example configuration can be found in this directory.
In `setup.py`, `cythonize` is called and compiler directives (see next section) can be passed.
Building can now be started with `python setup.py build_ext --inplace`.

# Name and describe two compiler directives in Cython.
A full overview of Cython compiler directives can be found [here](https://cython.readthedocs.io/en/latest/src/userguide/source_files_and_compilation.html#compiler-directives).

`language_level = <int>` specifies the targetted Python version.
As Cython produces compiled code, it must know what version of the Python interface to link against.
This also sets the language semantics of the Python code where it changed from 2 to 3.
`cdivision = <bool>` lets the user choose whether the Python or the C behaviour should be used for integer divisions and modulo operations.
Additionally to range checks, there are subtle differences in rounding, for instance.
See [CEP 516](https://github.com/cython/cython/wiki/enhancements-division) for further information.

# What is the difference between def, cdef and cpdef when declaring a Cython function?
- `def` defines a normal Python function.
  It can be called from both Python and Cython code.
  Additionally, it is allowed to specify C types for parameters, but not for the returned value, which is always a Python object.
- `cdef` defines variables with C types.
  It can also be used to define function that can only be called from Cython modules.
  Such functions specify a return type and types for their input arguments.
- `cpdef` function are a mix of the both above:
  They have a return type, but can be called from Python and Cython code.

# What are typed memoryviews especially useful for in Cython?
In Python (at least the CPython implementation), everything is a `PyObject`.
This sometimes creates unnecessary overhead when passing data between Python and Cython modules.
Typed memoryviews allow passing "raw" data without overhead for creating and (un-)packing `PyObject`s.
Deeper information on memoryviews can be found in [the official documentation](http://docs.cython.org/en/latest/src/userguide/memoryviews.html).
