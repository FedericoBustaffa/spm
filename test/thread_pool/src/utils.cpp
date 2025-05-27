#include "benchmarks.hpp"

#include <random>

std::vector<int> generate_numbers(size_t n, int min, int max)
{
    std::mt19937 rand_eng(42);
    std::uniform_int_distribution<int> dist(min, max);
    std::vector<int> values;
    values.reserve(n);

    for (size_t i = 0; i < n; i++)
        values.emplace_back(dist(rand_eng));

    return values;
}

int fibonacci(int n)
{
    if (n == 0 || n == 1)
        return n;

    return fibonacci(n - 1) + fibonacci(n - 2);
}
