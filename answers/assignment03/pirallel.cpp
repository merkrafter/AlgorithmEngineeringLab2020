#include <iomanip>
#include <iostream>
#include <array>
#include <omp.h>

using namespace std;

void integrate(const int from, const int to, const double width, double& result) {
  double sum = 0.0;
  for (int i = from; i < to; i++) {
    const double x = (i + 0.5) * width;      // midpoint
    sum += (1.0 / (1.0 + x * x)); // add new height of a rectangle
  }
  result = sum;
}

int main() {
  constexpr int NUM_THREADS = 1;
  // An array instead of a synchronized queue is sufficient if there will not be many threads as it is the case here
  std::array<double, NUM_THREADS> partial_sums;

  int num_steps = 100000000;              // amount of rectangles
  double width = 1.0 / double(num_steps); // width of a rectangle
  double start_time = omp_get_wtime(); // wall clock time in seconds

  integrate(0, num_steps, width, partial_sums[0]); // for summing up all heights of rectangles
  double sum = 0.0;
  for (int i = 0; i < partial_sums.size(); i++) {
    sum += partial_sums[i];
  }

  double pi = sum * 4 * width; // compute pi
  double run_time = omp_get_wtime() - start_time;

  cout << "pi with " << num_steps << " steps is " << setprecision(17) << pi
       << " in " << setprecision(6) << run_time << " seconds\n";
}
