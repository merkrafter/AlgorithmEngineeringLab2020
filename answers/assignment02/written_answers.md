# Coding assignment
## Fix race condition bug on page 13 with a `std::mutex`.
My implementation uses the `std::mutex` together with a `std::lock_guard`.
The latter takes ownership of the mutex and locks it if possible (else it blocks).
This approach has the advantage that the mutex is unlocked and released automatically as the `lock_guard` goes out of scope after the pi summation statement.

## Reduce runtime on page 16 by choosing an appropriate schedule.
Possible schedules for OpenMP are:
- *static*: approx. equal, predetermined number of iterations for each thread
- *dynamic*: divide iterations in predetermined (many, small) chunks;
  a thread requests a new chunk when it's done with the current one
- *guided*: similar to dynamic, but chunk sizes are determined dynamically at runtime
- *auto*: compiler or OpenMP runtime choose scheduling algorithm
- *runtime*: OpenMP runtime chooses scheduling algorithm; e.g. through environment variables

As the file name indicates, the workload is unbalanced.
Therefore, the static schedule is not the best option here as the iterations on one end of the for loop are much more computationally intense than on the other end.
This means that the first thread will be done quickly while all threads will have to wait for the last one in the end.
The following measured times show this property:
```
static 2.46052 seconds
dynamic: 1.25791 seconds
guided: 2.44866 seconds # I suppose the runtime overhead is too large
auto: 2.43807 seconds # probably chooses static or guided
runtime: 1.22664 seconds
```

Based on that, if the task is to choose a schedule option here, it would be `dynamic` or `runtime`.
**BUT** that is only half the story.
*If* the workload is unbalanced, but its characteristics are known, we can do even better.
In this example, as already mentioned, the workload of the images on one end of the vector is known to be much higher than on the other end.
It is therefore not the best idea to give one end to one tread and the other end to another one.
A better option would be passing to one thread images 0, 2, 4, ... and the ones with odd numbers to another (thread 2 would still have a higher workload, but it's a great improvement over the first approach).
```cpp
  constexpr int num_threads = 4;
#pragma omp parallel num_threads(num_threads)
  {
    const int thread_id = omp_get_thread_num();
#pragma omp for schedule(static) // denoise a bunch of images
    for (int i = thread_id; i < amount_images; i+=num_threads) {
      denoised_images[i] = denoise_image(images[i], i * i);
    }
  }
```
Note that this manual scheduling allows using `schedule(static)` for the parallel loop which avoids the runtime overhead of dynamic scheduling.
With that technique, the by far best time can be achieved:
```
static reordered: 0.656589 seconds
```
