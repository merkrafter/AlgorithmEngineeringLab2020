from setuptools import Extension, setup
from Cython.Build import cythonize

ext_modules = [
    # math library is not necessarily linked to automatically
    Extension("primes", sources=["primes.pyx"], libraries=["m"]) 
]

setup(
    ext_modules = cythonize(ext_modules, compiler_directives={"cdivision":True})
)
