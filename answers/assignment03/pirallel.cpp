#include <iomanip>
#include <iostream>
#include <omp.h>

using namespace std;

double integrate(const int from, const int to, const double width) {
  double sum = 0.0;
  for (int i = from; i < to; i++) {
    const double x = (i + 0.5) * width;      // midpoint
    sum += (1.0 / (1.0 + x * x)); // add new height of a rectangle
  }
  return sum;
}

int main() {
  int num_steps = 100000000;              // amount of rectangles
  double width = 1.0 / double(num_steps); // width of a rectangle
  double start_time = omp_get_wtime(); // wall clock time in seconds

  double sum = integrate(0, num_steps, width); // for summing up all heights of rectangles

  double pi = sum * 4 * width; // compute pi
  double run_time = omp_get_wtime() - start_time;

  cout << "pi with " << num_steps << " steps is " << setprecision(17) << pi
       << " in " << setprecision(6) << run_time << " seconds\n";
}
