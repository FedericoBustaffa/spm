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
                for (size_t j = 0; j < ranges.size(); j++)
                {
                    size_t length = ranges[j].second - ranges[j].first + 1;
                    size_t start =
                        (id * length) / workers_num + ranges[j].first;
                    size_t end =
                        ((id + 1) * length) / workers_num + ranges[j].first;

                    size_t counter = 0;
                    for (size_t k = start; k < end; k++)
                        counter += collatz_steps(k);

                    std::lock_guard<std::mutex> lock(steps_mtx);
                    steps[j] += counter;
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
