# Name some characteristics of the following instruction sets: SSE, AVX(2), AVX-512
|Property|SSE4.2|AVX2|AVX-512|
|----------|----|----|
|Vector length |128 | 256|512|
|Number of registers|16|16|32|
|Recommended alignment|16|32|64|

Note that vector instructions do not automatically mean that the run time of calculations is reduced relative to the fraction of data type size and vector length due to effects as [throttling](https://lemire.me/blog/2018/09/07/avx-512-when-and-how-to-use-these-new-instructions/).

# How can memory aliasing affect performance?
Memory aliasing means that two or more pointers refer to the same memory location.
This includes cases where at least two arrays overlap.
It can prevent loops from being auto-vectorized, as compilers must prove that vectorized code is semantically equivalent to the scalar version.
Consider this simple example:
```cpp
void add(float* a, const float* b, const int n) {
    for (int i = 0; i < n; ++i) {
        a[i] += b[i];
    }
}
```
This may look like perfectly vectorizable code, but the function does not prevent users from calling it like `add(a, a+1)`.
Note that now there are data dependencies between consecutive loop iterations that vector instructions can not deal with.
Therefore, the compiler needs a hint by the programmer, that the memory reasons do not overlap.
In C/C++, this is usually done with the `restrict` type qualifier (or respective compiler extensions).
If a pointer `p` is marked as `restrict`, all modifications to the memory region pointed to by `p` must happen through `p`.
In the example above, `a` can be marked with `__restrict__` to enable auto-vectorization of the loop.
This does not forbid users to pass aliased pointer, though.

The Rust compiler, on the other hand, does not need a separate keyword, as the type system prevents memory aliasing by design.
Calling this function with aliased pointers leads to a compile-time error.
Hence, the backend is safe to optimize this loop using vectorization by default.
```rust
fn add(dst_arr: &mut [f32], src_arr: &[f32]) {
    for (dst, src) in dst_arr.iter_mut().zip(src_arr.iter()) {
        *dst += src;
    }
}
```

# What are the advantages of unit stride memory access compared to accessing memory with larger strides?
Memory is said to be accessed *strided* if there is a constant distance between accessed memory locations.
This distance is called the stride, where *unit stride* refers to a stride of 1 (contiguous access).
Unit stride access makes it far easier for compilers to vectorize loops as the memory (e.g. an array) can be copied over to the vector registers and does not have to be gathered from locations that are far away from each other and then packed into the vector registers.

# When would you prefer arranging records in memory as Structure of Arrays?
The answer to this question is closely connected to the previous question: it depends on the memory access pattern.
If computation has to be done over a field of a struct for a whole array of those structs, the access to that field is strided with -- given that the struct has more than one field -- a non-unit stride.
Hence, it can hurt performance, because it might prevent auto-vectorization.
In that case, the data structure might better be represented as a structure of arrays, in which the field of interest lies contiguously in memory.
This, however, comes at the cost of slowing down access to elements *inside* a single struct, because its fields are now far away from each other.

The effect can be seen in the file `soa_vs_aos.cpp`.
Here, if one compares the assembly output of both functions, one can clearly see that the soa function is vectorized (`*ps` instructions) while the aos function is not (`*ss` instructions).

Interestingly, this behavior is affected by the fact that `Point` represents a point in 3 dimensions which is not a factor of the vector register size.
If `Point` is defined to be a 2D point, the `x` and `y` coordinates of `a` and `b` are packed into the vector registers and SIMD instructions will be used in this case.
The same can be observed in Rust, even when using a functional styled computation (`aos.rs`).
