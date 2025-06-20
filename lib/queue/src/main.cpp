#include <cstdio>
#include <thread>
#include <vector>

#include "AtomicQueue.hpp"
#include "LockQueue.hpp"

int main(int argc, char** argv)
{
    LockQueue<int> lq;
    AtomicQueue<int> aq;

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
