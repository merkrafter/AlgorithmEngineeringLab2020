#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

/*
 * Creates a random vector of length n filled with random
 * floats between 0 and 1.
 */
std::vector<float> get_random_float_vector(const int n) {
  std::default_random_engine re{std::random_device{}()};
  std::uniform_real_distribution<float> next_rand{0.0f, 1.0f};
  std::vector<float> v(n);
  for (auto &num : v) {
    num = next_rand(re);
  }
  return v;
}

/*
 * Computes the inner product of two vectors of size n with an unrolling factor
 * of K.
 */
template <int K>
float inner_product(const float *__restrict__ a, const float *__restrict__ b,
                    const int n);

template <>
float inner_product<1>(const float *__restrict__ a, const float *__restrict__ b,
                       const int n) {
  float result = 0;
  for (int i = 0; i < n; ++i) {
    result += a[i] * b[i];
  }
  return result;
}

template <>
float inner_product<4>(const float *__restrict__ a, const float *__restrict__ b,
                       const int n) {
  float result = 0;
  int i = 0;
  for (; i < n - 4 + 1; i += 4) {
    result += a[i] * b[i];
    result += a[i + 1] * b[i + 1];
    result += a[i + 2] * b[i + 2];
    result += a[i + 3] * b[i + 3];
  }
  for (; i < n; ++i) {
    result += a[i] * b[i];
  }
  return result;
}

int main() {
  constexpr int N = 1000000;
  // not divisible by unrolling factor to test correctness
  constexpr int LEN = 101;
  std::vector<float> v1 = get_random_float_vector(LEN);
  std::vector<float> v2 = get_random_float_vector(LEN);
  std::vector<float> results1(N);
  std::vector<float> results2(N);

  double start = omp_get_wtime();
  for (int i = 0; i < N; ++i) {
    results1[i] = inner_product<1>(v1.data(), v2.data(), LEN);
  }
  std::cout << "normal loop: " << omp_get_wtime() - start << " seconds"
            << std::endl;

  start = omp_get_wtime();
  for (int i = 0; i < N; ++i) {
    results2[i] = inner_product<4>(v1.data(), v2.data(), LEN);
  }
  std::cout << "unrolled loop: " << omp_get_wtime() - start << " seconds"
            << std::endl;

  // std::cout << result1 << std::endl;
  if (results1 != results2) {
    std::cout << "Error!" << std::endl;
  }
}
