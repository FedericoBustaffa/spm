#include "benchmarks.hpp"

#include "thread_pool.hpp"

std::vector<int> dynamic(const std::vector<int>& numbers)
{
    thread_pool tp(4);

    std::vector<std::future<int>> futures;
    futures.reserve(numbers.size());

    std::vector<int> out;
    out.reserve(numbers.size());

    for (const int& n : numbers)
        futures.push_back(tp.submit(fibonacci, n));

    for (std::future<int>& f : futures)
        out.push_back(f.get());

    return out;
}
