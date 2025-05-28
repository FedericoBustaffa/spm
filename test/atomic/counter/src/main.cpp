#include "atomic_counter.hpp"
#include "lock_counter.hpp"
#include "thread_pool.hpp"

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
        for (int i = 0; i < n; i++)
            lc.increment();

        return 0;
    };

    atomic_counter ac;
    auto atomic_increment = [&]() {
        for (int i = 0; i < n; i++)
            ac.increment();

        return 0;
    };

    thread_pool tp(w);
    std::vector<int> futures;
    futures.reserve(tp.size());

    for (size_t i = 0; i < tp.size(); i++)
        futures.tp.submit(lock_increment);

    for (size_t i = 0; i < tp.size(); i++)
        tp.submit(atomic_increment);

    return 0;
}
