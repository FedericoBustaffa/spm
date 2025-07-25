#include <atomic>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <vector>

#include "collatz.hpp"
#include "mpmc_queue.hpp"
#include "timer.hpp"

double dynamic(size_t workers_num, const range& range)
{
    std::vector<std::thread> workers;
    workers.reserve(workers_num);
    spm::mpmc_queue<uint64_t> buffer(range.length() * 2);

    std::atomic<uint64_t> counter(0);

    spm::timer timer;
    timer.start();
    for (size_t i = 0; i < workers_num; i++)
    {
        workers.emplace_back(
            [&](size_t id) {
                uint64_t local_counter = 0;
                std::optional<uint64_t> value;
                while (true)
                {
                    value = buffer.pop();
                    if (!value.has_value())
                        break;
                    else
                        local_counter += collatz_steps(value.value());
                }

                counter.fetch_add(local_counter);
            },
            i);
    }

    for (uint64_t i = range.a; i <= range.b; i++)
        buffer.push(i);

    buffer.close();

    for (auto& w : workers)
        w.join();
    double time = timer.stop();

    std::printf("dynamic steps: %lu\n", counter.load());

    return time;
}
