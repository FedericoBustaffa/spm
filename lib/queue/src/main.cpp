#include <cstdio>
#include <thread>
#include <vector>

#include "atomic_queue.hpp"
#include "lock_queue.hpp"

int main(int argc, char** argv)
{
    spm::lock_queue<int> lq;
    spm::atomic_queue<int> aq;

    auto produce = [&](int id) {
        for (int i = 0; i < 100; i++)
        {
            lq.push(id);
            aq.push(id);
        }
    };

    auto consume = [&]() {
        for (int i = 0; i < 100; i++)
        {
            lq.pop();
            aq.pop();
        }
    };

    std::vector<std::thread> workers;
    for (int i = 0; i < 20; i++)
    {
        workers.emplace_back(produce, i);
        workers.emplace_back(consume);
    }

    for (auto& w : workers)
        w.join();

    std::printf("lock queue capacity: %zu\n", lq.capacity());
    std::printf("lock queue size: %zu\n", lq.size());

    std::printf("atomic queue capacity: %zu\n", aq.capacity());
    std::printf("atomic queue size: %zu\n", aq.size());

    return 0;
}
