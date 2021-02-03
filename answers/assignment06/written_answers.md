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
