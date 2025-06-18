#include "collatz.hpp"

#include <chrono>

double sequential(const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& r : ranges)
    {
        for (uint64_t i = r.first; i < r.second; i++)
            collatz_steps(i);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    return duration.count();
}
