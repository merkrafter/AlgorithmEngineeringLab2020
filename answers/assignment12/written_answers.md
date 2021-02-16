# What are extension types in the context of Python?
Extension types are compiled C/C++ code that are wrapped by a comfortable
interface that make them look and feel like normal Python objects.
They are created by the `cdef class` keywords in a Cython module using a similar syntax to normal Python classes.
Important differences are listed below:

# How do extension types data fields in Cython differ from data fields in Python classes?
Cython's data fields can only be used from Cython code unless explicitly configured with access level modifiers.
They must be defined at the class level and may not be defined in some method as normal Python data fields are allowed to.

Apart from the differences in handling, access to them is as fast as in C.

# Give a simple description of how to wrap C / C++ code in Cython.
The overall process is very similar to the setuptools approach from the [last assignment](../assignment11).
Just as `math.h` is included, one can include any custom header file.
In `setup.py`, the C++ source and header files must be listed in the `sources` argument of an `Extension`.
The constructor also takes a `language` arg as well as `extra_compile_args` and `extra_link_args`.
It might be necessary to link to existing libraries using the `libraries`, `libraries_dir` and `include_dirs` arguments.
And that's it.
Now setuptools can build the whole project.
