#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <vector>

#include "threadpool.hpp"

std::vector<int> generate_numbers(size_t n, int min = 0, int max = 30);
std::vector<int> sequential(const std::vector<int>& numbers);
std::vector<int> submit(const std::vector<int>& numbers, spm::threadpool& pool);

#endif
