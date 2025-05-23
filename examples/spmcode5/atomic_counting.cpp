#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <hpc_helpers.hpp>

int main( ) {
    std::mutex mutex;
    std::vector<std::thread> threads;
    const uint64_t num_threads = 10;
    const uint64_t num_iters = 100'000'000;

    auto lock_count =
        [&] (uint64_t& counter, const auto& id) {
        for (uint64_t i = id; i < num_iters; i += num_threads) {
            std::lock_guard<std::mutex> lock_guard(mutex);
            counter++;
        }
    };
    auto atomic_count =
        [&] (std::atomic<uint64_t>& counter, const auto& id) {
			for (uint64_t i = id; i < num_iters; i += num_threads) {
				//counter++;
				//counter.fetch_add(1);
				std::atomic_fetch_add(&counter,1);
			}
    };
    TIMERSTART(mutex_multithreaded)
    uint64_t lock_counter = 0;
    threads.clear();
    for (uint64_t id = 0; id < num_threads; id++)
        threads.emplace_back(lock_count, std::ref(lock_counter), id);
    for (auto& thread : threads)
        thread.join();
    TIMERSTOP(mutex_multithreaded)

    TIMERSTART(atomic_multithreaded)
    std::atomic<uint64_t> atomic_counter(0);
    threads.clear();
    for (uint64_t id = 0; id < num_threads; id++)
        threads.emplace_back(atomic_count, std::ref(atomic_counter), id);
    for (auto& thread : threads)
        thread.join();
    TIMERSTOP(atomic_multithreaded)
    std::cout << lock_counter << " " << atomic_counter << std::endl;
}

