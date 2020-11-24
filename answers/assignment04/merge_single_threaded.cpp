#include <algorithm>
#include <iostream>
#include <omp.h>
#include <parallel/algorithm>
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

/*
 * algorithm for merge as described in:
 *
 * "Introduction to Algorithms" 3rd edition, p. 799
 */
template <class _Type, class _Compare>
inline int64_t my_binary_search(_Type value, const _Type *a, int left,
                                int right, _Compare __comp) {
  int64_t low = left;
  int64_t high = std::max(left, right + 1);
  while (low < high) {
    long mid = (low + high) / 2;
    if (__comp(value, a[mid]))
      high = mid;
    else
      low = mid + 1;
  }
  return high;
}

template <class _Type, class _Compare>
inline void merge_ptr(const _Type *a_start, const _Type *a_end,
                      const _Type *b_start, const _Type *b_end, _Type *dst,
                      _Compare __comp) {
  while (a_start < a_end && b_start < b_end) {
    if (__comp(*a_start, *b_start))
      *dst++ = *a_start++;
    else
      *dst++ = *b_start++;
  }
  while (a_start < a_end)
    *dst++ = *a_start++;
  while (b_start < b_end)
    *dst++ = *b_start++;
}

template <class _Type, class _Compare>
inline void merge_dac(_Type *t1, _Type *t2, int64_t p1, int64_t r1, int64_t p2,
                      int64_t r2, _Type *a, int64_t p3, _Compare __comp) {
  int64_t length1 = r1 - p1 + 1;
  int64_t length2 = r2 - p2 + 1;
  if (length1 < length2) {
    std::swap(p1, p2);
    std::swap(r1, r2);
    std::swap(length1, length2);
    std::swap(t1, t2);
  }
  if (length1 == 0)
    return;
  if ((length1 + length2) <= 8192 * 8) {
    merge_ptr(&t1[p1], &t1[p1 + length1], &t2[p2], &t2[p2 + length2], &a[p3],
              __comp);
  } else {
    int64_t q1 = (p1 + r1) / 2;
    int64_t q2 = my_binary_search(t1[q1], t2, p2, r2, __comp);
    int64_t q3 = p3 + (q1 - p1) + (q2 - p2);
    a[q3] = t1[q1];

    // TODO: parallelize the first recursive call with a task
    merge_dac(t1, t2, p1, q1 - 1, p2, q2 - 1, a, p3, __comp);
    merge_dac(t1, t2, q1 + 1, r1, q2, r2, a, q3 + 1, __comp);
  }
}

template <class _Type, class _Compare>
void parallel_merge(_Type *arr1, _Type *arr2, _Type *out, 
		  int64_t size1, int64_t size2, int n_threads, _Compare __comp) {
  // TODO: parallelize merge_dac, use n_threads as the amount of threads
  // for the parallel region
  merge_dac(arr1, arr2, 0, size1 - 1, 0, size2 - 1, out, 0, __comp);
}

int main(int argc, char *argv[]) {
  cout << "wait...\n";
  const int n = 50000000;
  vector<int> v1 = get_random_int_vector(n);
  vector<int> v2 = get_random_int_vector(n);

  // sort both vectors before merging
  __gnu_parallel::sort(begin(v1), end(v1));
  __gnu_parallel::sort(begin(v2), end(v2));

  // merge into these vectors
  vector<int> out1(2 * n);
  vector<int> out2(2 * n);
  vector<int> out3(2 * n);

  // test different merge implementations
  double start = omp_get_wtime();
  parallel_merge(v1.data(), v2.data(), out1.data(), n, n, omp_get_num_procs(), less<int>());
  cout << "custom merge: " << omp_get_wtime() - start << " seconds" << endl;

  start = omp_get_wtime();
  __gnu_parallel::merge(begin(v1), end(v1), begin(v2), end(v2), begin(out2));
  cout << "__gnu_parallel::merge: " << omp_get_wtime() - start << " seconds" << endl;

  start = omp_get_wtime();
  std::merge(begin(v1), end(v1), begin(v2), end(v2), begin(out3));
  cout << "std::merge: " << omp_get_wtime() - start << " seconds" << endl;

  if (out1 != out2 || out3 != out2) {
    cout << "merge implementation is buggy\n";
  }
}
