cdef extern from "math.h":
    cpdef double sqrt(double x)

def cython_is_prime(long z):
    if z < 2:
        return False
    if z == 2:
        return True
    if z % 2 == 0:
        return False

    cdef int i
    cdef int upper = int(sqrt(z))+1
    for i in range(3, upper, 2):
        if z % i == 0:
            return False
    return True
