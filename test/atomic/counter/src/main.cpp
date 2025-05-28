#include <cassert>
#include <cstdio>

#include "atomic_counter.hpp"
#include "lock_counter.hpp"
#include "thread_pool.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
    size_t e = 15;
    if (argc >= 2)
        e = std::atol(argv[1]);
    size_t n = 1 << e;

    size_t w = 0;
    if (argc >= 3)
        w = std::atol(argv[2]);

    lock_counter lc;
    auto lock_increment = [&]() {
        for (size_t i = 0; i < n; i++)
            lc.increment();

        return 0;
    };

    atomic_counter ac;
    auto atomic_increment = [&]() {
        for (size_t i = 0; i < n; i++)
            ac.increment();

        return 0;
    };

    thread_pool tp(w);
    Timer timer;

    std::vector<std::future<int>> futures;
    futures.reserve(tp.size());

    // lock
    timer.start();
    for (size_t i = 0; i < tp.size(); i++)
        futures.push_back(tp.submit(lock_increment));

    for (size_t i = 0; i < tp.size(); i++)
        futures[i].get();
    double lock_time = timer.stop();
    std::printf("lock time: %.2f\n", lock_time);

    futures.clear();
    futures.reserve(tp.size());

    // atomic
    timer.start();
    for (size_t i = 0; i < tp.size(); i++)
        futures.push_back(tp.submit(atomic_increment));

    for (size_t i = 0; i < tp.size(); i++)
        futures[i].get();
    double atomic_time = timer.stop();
    std::printf("atomic time: %.2f\n", atomic_time);

    std::printf("atomic speedup: %.2f\n", lock_time / atomic_time);

    std::printf("lock counter: %ld\n", lc.get());
    std::printf("atomic counter: %ld\n", ac.get());

    assert(lc.get() == ac.get());

    return 0;
}
