#include <atomic>
#include <thread>
#include <vector>

#include "collatz.hpp"

uint64_t block_cyclic(size_t workers_num, size_t chunksize, const range& range)
{
    // pool of workers
    std::vector<std::thread> workers;
    workers.reserve(workers_num);

    // global steps counter
    std::atomic<uint64_t> counter(0);

    for (size_t w = 0; w < workers_num; w++)
    {
        workers.emplace_back(
            [&](size_t id) {
                uint64_t local_counter = 0;
                for (uint64_t i = range.a + id * chunksize; i <= range.b;
                     i += workers_num * chunksize)
                {
                    for (uint64_t j = i;
                         j < std::min(i + chunksize, range.b + 1); j++)
                    {
                        // std::printf("thread %zu compute on %lu\n", id, j);
                        local_counter += collatz_steps(j);
                    }
                }

                counter.fetch_add(local_counter);
            },
            w);
    }

    for (auto& w : workers)
        w.join();

    return counter.load();
}
