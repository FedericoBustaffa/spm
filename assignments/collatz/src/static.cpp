#include <chrono>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

#include "collatz.hpp"

double static_schedule(size_t workers_num,
                       const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    // results vector
    std::vector<uint64_t> steps;
    steps.reserve(ranges.size());
    for (size_t i = 0; i < ranges.size(); i++)
        steps.emplace_back(0);
    std::mutex steps_mtx;

    // pool of workers
    std::vector<std::thread> workers;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < workers_num; i++)
    {
        workers.emplace_back(
            [&](size_t id) {
                size_t chunksize;
                for (size_t i = 0; i < ranges.size(); i++)
                {
                    chunksize = std::floor(
                        (ranges[i].second - ranges[i].first) / workers_num);

                    uint64_t start = id * chunksize + ranges[i].first;
                    for (uint64_t j = start;
                         j < std::min(start + chunksize, ranges[i].second); j++)
                    {
                        std::lock_guard<std::mutex> lock(steps_mtx);
                        steps[i] += collatz_steps(j);
                    }
                }
            },
            i);
    }
    for (auto& w : workers)
        w.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    for (size_t i = 0; i < ranges.size(); i++)
    {
        std::printf("%lu-%lu: %lu steps\n", ranges[i].first, ranges[i].second,
                    steps[i]);
    }

    return duration.count();
}
