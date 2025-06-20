#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

#include "collatz.hpp"
#include "lock_queue.hpp"

double dynamic(size_t workers_num,
               const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    std::atomic<uint64_t>* steps = new std::atomic<uint64_t>[ranges.size()];

    std::vector<std::thread> workers;
    workers.reserve(workers_num);

    spm::lock_queue<std::pair<size_t, uint64_t>> buffer;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < workers_num; i++)
    {
        workers.emplace_back(
            [&](size_t id) {
                std::optional<std::pair<size_t, uint64_t>> idx_value;
                while (true)
                {
                    idx_value = buffer.pop();
                    if (!idx_value.has_value())
                        return;
                    else
                    {
                        steps[idx_value.value().first] +=
                            collatz_steps(idx_value.value().second);
                    }
                }
            },
            i);
    }

    for (size_t i = 0; i < ranges.size(); i++)
    {
        for (uint64_t j = ranges[i].first; j <= ranges[i].second; j++)
            buffer.push({i, j});
    }

    buffer.close();

    for (auto& w : workers)
        w.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    for (size_t i = 0; i < ranges.size(); i++)
    {
        std::printf("%lu-%lu: %lu steps\n", ranges[i].first, ranges[i].second,
                    steps[i].load());
    }

    delete[] steps;

    return duration.count();
}
