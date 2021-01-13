# Name some characteristics of the following instruction sets: SSE, AVX(2), AVX-512
|Property|SSE4.2|AVX2|AVX-512|
|----------|----|----|
|Vector length |128 | 256|512|
|Number of registers|16|16|32|
|Recommended alignment|16|32|64|

Note that vector instructions do not automatically mean that the run time of calculations is reduced relative to the fraction of data type size and vector length due to effects as [throttling](https://lemire.me/blog/2018/09/07/avx-512-when-and-how-to-use-these-new-instructions/).
