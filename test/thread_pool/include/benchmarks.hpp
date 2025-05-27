#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <vector>

std::vector<int> generate_numbers(size_t n, int min = 0, int max = 30);

int fibonacci(int n);

std::vector<int> sequential(const std::vector<int>& numbers);
std::vector<int> block(const std::vector<int>& numbers);
std::vector<int> cyclic(const std::vector<int>& numbers, size_t chunksize = 1);
std::vector<int> dynamic(const std::vector<int>& numbers);

#endif
