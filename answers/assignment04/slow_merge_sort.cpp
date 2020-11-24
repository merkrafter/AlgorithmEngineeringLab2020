#include <algorithm>
#include <cstring>
#include <iostream>
#include <omp.h>
#include <random>
using namespace std;

vector<int> get_random_int_vector(int n) {
  default_random_engine re{random_device{}()};
  uniform_int_distribution<int> next_rand{INT32_MIN, INT32_MAX};
  vector<int> v(n);
  for (auto &num : v) {
    num = next_rand(re);
  }
  return v;
}

inline void merge(const int *__restrict__ a, const int *__restrict__ b,
                  int *__restrict__ c, const int a_size, const int b_size,
                  const int c_size) {
  int idx_a = 0;
  int idx_b = 0;
  for (int i = 0; i < c_size; ++i) {
    if (idx_a == a_size) {
      c[i] = b[idx_b++];
    } else if (idx_b == b_size) {
      c[i] = a[idx_a++];
    } else {
      c[i] = (a[idx_a] < b[idx_b]) ? a[idx_a++] : b[idx_b++];
    }
  }
}

void merge_sort_naive(int *arr, int n) { // slow merge sort
  if (n > 1) { // TODO: use insertion sort for small n
    const int size_a = n / 2;
    const int size_b = n - size_a;
    // TODO: make next recursive call a task
    merge_sort_naive(arr, size_a); // recursive call
    merge_sort_naive(arr + size_a, size_b); // recursive call
    // TODO: here should be a taskwait
    int *c = new int[n]; // TODO: avoid using heap for small n
    merge(arr, arr + size_a, c, size_a, size_b, n);
    memcpy(arr, c, sizeof(int) * n);
    delete[](c);
  }
}

int main(int argc, char *argv[]) {
  const int n = 100000000;
  vector<int> v = get_random_int_vector(n);
  vector<int> v_copy = v;

  double start = omp_get_wtime();
  merge_sort_naive(v.data(), n);
  cout << "naive: " << omp_get_wtime() - start << " seconds" << endl;

  start = omp_get_wtime();
  sort(begin(v_copy), end(v_copy));
  cout << "std::sort: " << omp_get_wtime() - start << " seconds" << endl;

  if (v != v_copy) {
    cout << "sort implementation is buggy\n";
  }
}
