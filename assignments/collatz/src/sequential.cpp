#include "collatz.hpp"

#include <chrono>

double sequential(const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    std::vector<uint64_t> steps;
    steps.reserve(ranges.size());

    uint64_t counter = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& r : ranges)
    {
        for (uint64_t i = r.first; i < r.second; i++)
            counter += collatz_steps(i);

        steps.emplace_back(counter);
        counter = 0;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    for (size_t i = 0; i < ranges.size(); i++)
    {
        std::printf("%lu-%lu: %lu steps\n", ranges[i].first, ranges[i].second,
                    steps[i]);
    }

    return duration.count();
}
