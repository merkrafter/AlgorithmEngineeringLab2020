# Explain three vectorization clauses that can be used with `#pragma omp simd`
## Alignment
The alignment can be specified with `aligned(var_list: num_bytes)`.
Vector operations require aligned input data as pointed out in the last assignment.
Hence, unaligned input arrays must be peeled off at the beginning before vector operations can be used.
How much peeling is necessary depends on the vector width.
Passing the (in other parts of the program) ensured alignment can help the compiler make better decisions, or can remove the necessecity to peel off altogether.
## Vector length preference
- `simdlen` suggests a number of concurrently processed elements to the compiler.
  Suboptimal choices will not lead to false results (compared to the simd pragma without a `simdlen` clause).
  It usually is not necessary to specify this.
 - `safelen` sets an upper bound for the number of concurrently processed elements.
   This can be necessary to ensure correctness of a vectorized implementation.

## Data-sharing attributes
The data-sharing attributes for `#pragma omp simd` are actually the same as for the parallel pragma.
Specifically, the important clauses `private`, `firstprivate` and `reduction` work as described in [assignment 03](../assignment03/written_answers.md#clarify-how-the-storage-attributes-private-and-firstprivate-differ-from-each-other).

# Give reasons that speak for and against vectorization with intrinsics compared to guided vectorization with OpenMP
OpenMP pragmas have several advantages over intrinsics when it comes to vectorization.
First, they make the code platform portable.
The same `#pragma omp simd`-annotated for loop can be compiled for the latest server hardware and small single-board computers without modification.
Intrinsics may not be available and compilable on all processor architectures.
Also, they are generally safer, because they *can* be ignored by the compiler if it has concerns, e.g. in case of memory aliasing.
There is no guarantee, though, and in fact, the compiler can see a misplaced `#pragma omp simd` as a reason to vectorize incorrectly.
(On the other hand, intrinsics likewise do not prevent bugs with data dependencies.)

The advantages of intrinsics lie in performance portability:
OMP's pragmas can be ignored by compilers, and can even lead to slower code on some platforms.
But the compiler can not ignore intrinsics as they are actual functions, hence different compilers will produce similarly performing programs.
Additionally, compilers might fail to vectorize even safe cases annotated with an OMP pragma, if they are too complex.
Intrinsics give the programmer the full control over the vector capabilities of processors, including a richer instruction set.
For instance, AVX-512 defines a max reduction instruction (e.g. `_mm512_reduce_max_epu64`).
This freedom allows to vectorize even complex algorithms such as sorting nets.

# What are the advantages of vector intrinsics over assembly code?
The main advantage is that vector intrinsics are easier to learn, use and read.
For instance, try to understand what this code does:
```asm
vshuff32x4 zmm1, zmm0, zmm0, 238
vpmaxuq   zmm2, zmm1, zmm0
vpermpd   zmm3, zmm2, 78
vpmaxuq   zmm4, zmm2, zmm3
vpermpd   zmm5, zmm4, 177
vpmaxuq   zmm6, zmm4, zmm5
vmovq     rax, xmm6
vzeroupper
ret
```
The `vpmaxuq` instructions might give a hint that this is the compiled version of the abovementioned `_mm512_reduce_max_epu64` intrinsic, which is *much* easier to understand in a program.
By using intrinsics, the programmer also does not have to choose a concrete register allocation.
Finally, as intrinsics are valid C functions, they are portable between different compilers and operating systems.

# What are the corresponding vectors of the three intrinsic data types: `__m256`, `__m256d` and `__m256i`?
The intrinsic data types make a distinction between different number formats.
- `__m256` uses the single-precision floating-point number format
- `__m256d` uses the double-precision floating-point number format
- `__m256i` uses integer formats; signedness and bitlengths is explained in the [next assignment](../assignment08/written_answers.md)
