#include "hpc_helpers.hpp"
#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

int main()
{
    std::vector<std::thread> threads;
    const uint64_t num_threads = 10;
    const uint64_t num_iters = 100'000'000;

    // WARNING: false_max produces incorrect results
    auto false_max = [&](std::atomic<uint64_t>& counter,
                         const auto& id) -> void {
        for (uint64_t i = id; i < num_iters; i += num_threads)
            if (i > counter)
                counter = i;
    };

    auto correct_max = [&](std::atomic<uint64_t>& counter,
                           const auto& id) -> void {
        for (uint64_t i = id; i < num_iters; i += num_threads)
        {
            auto previous = counter.load();
            // if CAS fails, previous is updated with the content of counter
            while (previous < i && !counter.compare_exchange_weak(previous, i))
                ;
        }
    };

    TIMERSTART(incorrect_max);
    std::atomic<uint64_t> false_counter(0);
    threads.clear();
    for (uint64_t id = 0; id < num_threads; id++)
        threads.emplace_back(false_max, std::ref(false_counter), id);

    for (auto& thread : threads)
        thread.join();
    TIMERSTOP(incorrect_max);

    TIMERSTART(correct_max);
    std::atomic<uint64_t> correct_counter(0);
    threads.clear();
    for (uint64_t id = 0; id < num_threads; id++)
        threads.emplace_back(correct_max, std::ref(correct_counter), id);

    for (auto& thread : threads)
        thread.join();
    TIMERSTOP(correct_max);
    std::cout << false_counter << " " << correct_counter << std::endl;
}
