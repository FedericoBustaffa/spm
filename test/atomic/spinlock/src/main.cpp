#include <cassert>
#include <cstdio>

#include "atomic_counter.hpp"
#include "lock_counter.hpp"
#include "spin_counter.hpp"
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

    spin_counter sc;
    auto spin_increment = [&]() {
        for (size_t i = 0; i < n; i++)
            sc.increment();

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

    futures.clear();
    futures.reserve(tp.size());

    // spin
    timer.start();
    for (size_t i = 0; i < tp.size(); i++)
        futures.push_back(tp.submit(spin_increment));

    for (size_t i = 0; i < tp.size(); i++)
        futures[i].get();
    double spin_time = timer.stop();
    std::printf("spin time: %.2f\n", spin_time);

    std::printf("atomic speedup: %.2f\n", lock_time / atomic_time);
    std::printf("spin speedup: %.2f\n", lock_time / spin_time);

    std::printf("lock counter: %ld\n", lc.get());
    std::printf("atomic counter: %ld\n", ac.get());
    std::printf("spin counter: %ld\n", sc.get());

    assert(lc.get() == ac.get());
    assert(lc.get() == sc.get());

    return 0;
}
