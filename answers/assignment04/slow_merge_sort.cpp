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

void insertion_sort(int *arr, const int n, int *out) {
  for (int i = 0; i < n; i++) {
    const int curr_val = arr[i];
    int j = i;
    while (j > 0 && out[j-1] > curr_val) {
      out[j] = out[j-1];
      arr[j] = arr[j-1];
      j--;
    }
    out[j] = curr_val;
    arr[j] = curr_val;
  }
}

void merge_sort_tasked_prealloc(int *arr, int n, int *buffer) { // slow merge sort
  if (n > 1) {
    if (n < 32) {
      insertion_sort(arr, n, buffer);
      return;
    }
    const int size_a = n / 2;
    const int size_b = n - size_a;

    // attention: swap roles of buffer and arr below to
    // avoid copies from buffer to arr
    #pragma omp task if (n > 5000)
    merge_sort_tasked_prealloc(buffer, size_a, arr); // recursive call
    merge_sort_tasked_prealloc(buffer + size_a, size_b, arr + size_a); // recursive call
    #pragma omp taskwait
    merge(arr, arr + size_a, buffer, size_a, size_b, n);
  }
}

void merge_sort(int *arr, int n) {
int *buffer = new int[n];
#pragma omp parallel
#pragma omp single nowait
  merge_sort_tasked_prealloc(arr, n, buffer);
memcpy(arr, buffer, sizeof(int) * n);
delete[](buffer);
}

int main(int argc, char *argv[]) {
  const int n = 100000000;
  vector<int> v = get_random_int_vector(n);
  vector<int> v_copy = v;

  double start = omp_get_wtime();
  merge_sort(v.data(), n);
  cout << "tasked: " << omp_get_wtime() - start << " seconds" << endl;

  start = omp_get_wtime();
  sort(begin(v_copy), end(v_copy));
  cout << "std::sort: " << omp_get_wtime() - start << " seconds" << endl;

  if (v != v_copy) {
    cout << "sort implementation is buggy\n";
  }
}
