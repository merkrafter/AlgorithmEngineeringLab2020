#include <iostream>
#include <omp.h> // required for library routines

using namespace std;
int main() {
#pragma omp parallel // compiler directive
  {
    // omp_get_thread_num() is an OpenMP library routine
    auto thread_id = omp_get_thread_num();
    cout << "Hello from thread " << thread_id << endl;
  }
}
