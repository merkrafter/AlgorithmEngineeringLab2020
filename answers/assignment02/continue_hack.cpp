#include <omp.h>
#include <atomic>
#include <iostream>

using namespace std;

bool is_solution(int number) {  // test if number solves the problem
  for (volatile int i = 10000000; i--;) {}  // mock computation
  return number > 133 && number < 140;
}

int get_solution_original() {
  constexpr int biggest_possible_number = 10000;
  // To avoid undefined code, it is necessary to use std::atomic for variables 
  // where race conditions may happen.
  // https://databasearchitects.blogspot.com/2020/10/c-concurrency-model-on-x86-for-dummies.html
  atomic<bool> solution_found(false);  // if true than we found the solution
  atomic<int> final_solution(INT32_MAX);

#pragma omp parallel for schedule(dynamic)  // start parallel region
  for (int i = 0; i < biggest_possible_number; ++i) {
    if (solution_found)  // we found the solution, just continue iterating
      continue;
    if (is_solution(i)) {     // find some solution, not necessary the smallest
      solution_found = true;
      final_solution = i;
    }
  }  // end parallel region
  return final_solution;
}

int get_solution_min() {
  constexpr int biggest_possible_number = 10000;
  atomic<int> final_solution(INT32_MAX);

#pragma omp parallel for schedule(dynamic)  // start parallel region
  for (int i = 0; i < biggest_possible_number; ++i) {
    if(final_solution < i) {
        continue;
    }
    if (is_solution(i)) {     // find some solution, not necessary the smallest
      int previous = final_solution.load();
      while(previous > i && !final_solution.compare_exchange_weak(previous,i)) {}
    }
  }  // end parallel region
  return final_solution;
}

int main() {
  double start = omp_get_wtime();
  int final_solution = get_solution_original();
  cout << "original:     The solution is: " << final_solution << endl;
  cout << omp_get_wtime() - start << " seconds" << endl;

  start = omp_get_wtime();
  final_solution = get_solution_min();
  cout << "min:          The solution is: " << final_solution << endl;
  cout << omp_get_wtime() - start << " seconds" << endl;
}
