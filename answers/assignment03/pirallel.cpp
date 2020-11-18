#include <array>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <thread>

using namespace std;

// Returns the integral of the pi generating function from x=from to x=to,
// exclusively using the specified width. The result is stored in the given
// reference.
void integrate(const int from, const int to, const double width,
               double &result) {
  // (somewhat) mitigate false sharing by avoiding writes to the result in every
  // single iteration
  double sum = 0.0;
  for (int i = from; i < to; i++) {
    const double x = (i + 0.5) * width; // midpoint
    sum += (1.0 / (1.0 + x * x));       // add new height of a rectangle
  }
  result = sum;
}

int main() {
  constexpr int NUM_THREADS = 2;
  // An array instead of a synchronized queue is sufficient if there will not be
  // many threads as it is the case here
  // -1 since the main thread will calculate a piece as well
  std::array<std::thread, NUM_THREADS - 1> threads;
  std::array<double, NUM_THREADS - 1> partial_sums;

  int num_steps = 100000000;              // amount of rectangles
  double width = 1.0 / double(num_steps); // width of a rectangle
  double start_time = omp_get_wtime();    // wall clock time in seconds

  for (int i = 0; i < threads.size(); i++) {
    const int start = i * num_steps / NUM_THREADS;
    const int end = (i + 1) * num_steps / NUM_THREADS;
    threads[i] =
        std::thread(integrate, start, end, width, std::ref(partial_sums[i]));
  }
  // main threads computes the last chunk to avoid rounding errors
  const int start = (NUM_THREADS - 1) * num_steps / NUM_THREADS;
  const int end = num_steps;
  double sum;
  integrate(start, end, width, sum);

  for (int i = 0; i < threads.size(); i++) {
    threads[i].join();
  }

  for (int i = 0; i < partial_sums.size(); i++) {
    sum += partial_sums[i];
  }

  double pi = sum * 4 * width; // compute pi
  double run_time = omp_get_wtime() - start_time;

  cout << "pi with " << num_steps << " steps is " << setprecision(17) << pi
       << " in " << setprecision(6) << run_time << " seconds\n";
}
