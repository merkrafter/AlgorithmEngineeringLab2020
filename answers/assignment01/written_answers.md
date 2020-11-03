# Describe how parallelism differs from concurrency.
Concurrency allows the execution of multiple tasks.
Parallelism is a subtype of concurrency that allows the execution of multiple tasks at the very same point in time.

# What is fork-join parallelism?
In fork-join parallelism, there are points where a program, for instance, starts parallel work (fork) and others where the parallelism ends and a sequential part begins (join).

# Herb Sutter's article *The free lunch is over*
## What new CPU technologies does Sutter report?
"Processor manufacturers [...] are [...] turning [...] to hyperthreading and multicore architectures"
## Can we have a free lunch again if we exploit them in our applications?
In my opinion, a general free lunch is not possible anymore.
I do think, however, that we can have a pretty cheap lunch.
One reason for this is the growth of caches.
Here, programs that needed more memory than would fit in the cache in the past, can take advantage of the drastically lower access times that caches offer compared to RAM.
Multiprocessor architectures and hyperthreading can speed up applications that are parallel(izable).
Unfortunately, this is not given for every application, as the typical read-execute-write system is sequential (especially if the "execute" is some sort of aggregation operation).
On the other hand, I claim most applications can use some kind of parallelism to their advantage in a similar way applications took advantage of rising clock cycles, that is applications written and compiled today get better with the hardware of tomorrow.
Let's consider a HTTP webserver that processes client requests (it could read some data from a data base) and sends back an answer.
Surely parallelism does not help in processing that one request in the free lunch context.
Maybe there are 2 or 3 things that can be done in parallel, but that does not scale infinitely.
What *does* scale perfectly, however, is the number of requests the server can handle in parallel.
For a well-written server application, buying better hardware can definitely pay off.

One argument against the *free* lunch is that unleashing the power of parallelism requires extra effort during the design and implementation phase of applications.
Obviously that is true during the design phase.
No computer can auto-parallelize your program without understanding the semantics of the underlying problem.
It is not necessarily true for the implementation phase, as parallilization macros/annotations and libraries are getting more and more powerful.
As can be seen below in the practical programming section, adding a single `#pragma omp parallel for` can dramatically increase the performance of the application.
Of course that's not completely free, but quite a cheap lunch.

# Discuss one thing you find particularly interesting in *Computer System: A programmer's perspective*
The one thing I find most interesting is avoiding security holes, mentioned on page 5.
Here I want to focus on buffer overflows.
Consider the following function
```C
void chk_password() {
    char read_buf[0xff];

    printf("Enter your password: ");
    gets(read_buf);
    if(strcmp(read_buf, "sUp3r$ecrETp4zzw0rd") == 0) {
        printf("Correct; go on\n");
    } else {
        printf("Wrong password!\n");
        _exit(0);
    }
}
```
This function allocates 255 bytes of memory on the stack, reads the user input into it and checks whether the user input is correct (of course, in a real application the input would be hashed and compared to a stored hashed value, but let's keep it simple).
As the man page of `gets` says, it reads characters from `stdin` into the passed buffer until a newline character appears, "No check for buffer overrun is performed".
This means that any input longer than 255 bytes will overwrite data behind the buffer, especially the return address is vulnerable.
If an attacker can write arbitrary values to the return address, he can control the program's execution flow, calling dangerous functions as `system`.

The first (apparent) line of defense in this special case is the password check.
It might appear as if someone passes a superlong string, the `strcmp` fails and the program exits.
However, in C, strings are terminated by a zero byte, so `strcmp` only checks the first bytes up to the point where it encounters a zero byte.
An attacker can now pass in the string `sUp3r$ecrETp4zzw0rd\x00` followed by some more data that overflows the buffer and overwrites the return address as desired and it will work.

An effective second line of defense that is enabled by default nowadays is called stack canary.
The idea is to place some random data on the stack in front of the return address.
This value, the canary, is then checked for modification before the function performs a return.
It can be spotted in the assembly with the following instructions:
```assembly
<+11>:    mov    rax,QWORD PTR fs:0x28    ; load canary value
<+20>:    mov    QWORD PTR [rbp-0x8],rax  ; store it on the stack
; do the main functionality
<+96>:    mov    rax,QWORD PTR [rbp-0x8]  ; load the canary from stack
<+100>:   sub    rax,QWORD PTR fs:0x28    ; compare it with the value it had before
<+109>:   je     0x401200 <chk_password+138> ; continue to return point if canary was okay
<+111>:   call   0x401050 <__stack_chk_fail@plt> ; issue a warning in case the canary was hurt
```
Since the canary value is random, an attacker can not guess it.
He can crash it and hence threaten the availability, but he cannot exploit the program.
Often, the canary is global for the program.
Through other vulnerabilities as format strings, an attacker could leak the canary value and use it in his exploit string.

# Coding assignment
## Warm up
To parallelize the pi program on slide 17, I used the OMP directive `reduction`.
It can be seen in commit `28fe5dcd76e251f`.
This way, I avoid synchronization of each single access to a shared `sum` variable.
Instead, each thread now has an own `sum` variable that is summed up with the ones from other threads in the end.
Result before parallelization:
```bash
$> make pirallel
$> ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.768738 seconds
```
Result after parallelization:
```bash
$> make pirallel
$> OMP_NUM_THREADS=1 ./pirallel
pi with 100000000 steps is 3.1415926535904264 in 0.764026 seconds
$> OMP_NUM_THREADS=2 ./pirallel  # ~ 0.5 time from above
pi with 100000000 steps is 3.1415926535899099 in 0.382338 seconds
$> OMP_NUM_THREADS=4 ./pirallel  # does not give any advantage on my machine
pi with 100000000 steps is 3.1415926535896825 in 0.383342 seconds
```

```bash
$> make pirallel_opt
$> OMP_NUM_THREADS=1 ./pirallel_opt # it's as fast as the dual-thread version above
pi with 100000000 steps is 3.1415926535900236 in 0.384309 seconds
$> OMP_NUM_THREADS=2 ./pirallel_opt # again, ~ 0.5 of the single-threaded time
pi with 100000000 steps is 3.1415926535898353 in 0.196232 seconds
```

# Parallel Pi estimation through Monte Carlo simulation
Here, the parallelization was introduced in commit `ccac30834f19ce`.
The diff is cluttered due to the formatter, but there are 2 essential parts:
1. Start a parallel region including the random generator creation. This prevents every thread from using the same seed.
The `counter` is, again, reduced by summation over the whole region.
2. As before, the `for` loop is parallelized.
Using these changes, the below results could be achieved:
```bash
make pirallel_mc
$> OMP_NUM_THREADS=1 ./pirallel_mc
pi: 3.14162
run_time: 17.157 s
n: 100000000
$> OMP_NUM_THREADS=2 ./pirallel_mc
pi: 3.14167
run_time: 9.04005 s
n: 100000000
```
This time, the multi-threaded version is not exactly twice as fast as the single threaded one.
A possible cause for this is that there is more to do than the parallelizable loop.
```bash
$> make pirallel_mc_opt
$> OMP_NUM_THREADS=2 ./pirallel_mc_opt
pi: 3.1415
run_time: 1.70091 s
n: 100000000
```
This time, the optimized version is about 5 times as fast as without `-Ofast`.
