#include <iostream>
#include <omp.h>
#include <sstream>

using namespace std;

int main() {
#pragma omp parallel num_threads(4)
  {
    // each thread has its own stringstream instance
    stringstream thread_iterations;
    // task for you: experiment with different schedules
#pragma omp for schedule(static, 22)
    for (int i = 0; i < 32; ++i) {
      // write the iterations of a thread to the stream
      thread_iterations << i << " ";
    }
    // only one thread is allowed to output to the console
#pragma omp critical
      cout << "thread " << omp_get_thread_num()
           << " computed the iterations: "
           << thread_iterations.str() << endl;
  }
}
