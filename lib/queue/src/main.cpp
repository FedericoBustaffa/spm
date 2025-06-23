#include <cstdio>
#include <thread>
#include <vector>

#include "lock_queue.hpp"
#include "mpmc_queue.hpp"
#include "spsc_queue.hpp"

int main(int argc, char** argv)
{
    uint64_t e = 10;
    if (argc >= 2)
        e = std::stoul(argv[1]);

    uint64_t n = 1 << e;
    std::printf("n: %lu\n", n);

    spm::lock_queue<int> lq;
    spm::spsc_queue<int> spsc;
    spm::mpmc_queue<int> mpmc;

    auto produce = [&](int id) {
        for (uint64_t i = 0; i < n; i++)
        {
            lq.push(id);
            spsc.push(id);
            mpmc.push(id);
        }
    };

    auto consume = [&]() {
        for (uint64_t i = 0; i < n; i++)
        {
            lq.pop();
            spsc.pop();
            mpmc.pop();
        }
    };

    std::vector<std::thread> workers;
    for (int i = 0; i < 2; i++)
    {
        workers.emplace_back(produce, i);
        workers.emplace_back(consume);
    }

    for (auto& w : workers)
        w.join();

    std::printf("lock size: %zu\n", lq.size());
    std::printf("spsc size: %zu\n", spsc.size());
    std::printf("mpmc size: %zu\n", mpmc.size());

    return 0;
}
