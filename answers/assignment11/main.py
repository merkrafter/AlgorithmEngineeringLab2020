"""
build with: python setup.py build_ext --inplace
run with: python main.py NUMBER
"""
import argparse
from collections import namedtuple
from math import sqrt
from time import time

import primes

Candidate = namedtuple("Candidate", ["name", "implementation"])

def python_is_prime(z):
    if z < 2:
        return False
    if z == 2:
        return True
    if z % 2 == 0:
        return False
    upper = int(sqrt(z))+1
    for i in range(3, upper, 2):
        if z % i == 0:
            return False
    return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("number", type=int, help="prime number candidate")
    args = parser.parse_args()

    candidates = [
        Candidate("Python", python_is_prime),
        Candidate("Cython", primes.cython_is_prime),
    ]

    print_times_for(args.number, candidates)

def print_times_for(z, candidates):
    """
    Candidates are functions that take a single integer and return whether it
    is a prime number as a boolean value.
    """
    for candidate in candidates:
        start = time()
        print(candidate.name, candidate.implementation(z), end="")
        end = time()
        print(f" {end-start}s")

if __name__ == "__main__":
    main()
