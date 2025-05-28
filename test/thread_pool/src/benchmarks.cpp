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

std::vector<int> sequential(const std::vector<int>& numbers)
{
    std::vector<int> out;
    out.reserve(numbers.size());

    for (const auto& n : numbers)
        out.emplace_back(fibonacci(n));

    return out;
}

std::vector<int> submit(const std::vector<int>& numbers, thread_pool& pool)
{

    std::vector<std::future<int>> futures;
    futures.reserve(numbers.size());

    std::vector<int> out;
    out.reserve(numbers.size());

    for (const int& n : numbers)
        futures.push_back(pool.submit(fibonacci, n));

    for (std::future<int>& f : futures)
        out.emplace_back(f.get());

    return out;
}

std::vector<int> submit_async(const std::vector<int>& numbers,
                              thread_pool& pool)
{
    std::vector<std::future<int>> futures;
    futures.reserve(numbers.size());

    std::vector<int> out;
    out.reserve(numbers.size());

    for (const int& n : numbers)
        futures.push_back(pool.submit_async(fibonacci, n));

    for (std::future<int>& f : futures)
        out.emplace_back(f.get());

    return out;
}
