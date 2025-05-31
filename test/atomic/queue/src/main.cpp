#include <cstdio>
#include <thread>
#include <vector>

#include "AtomicQueue.hpp"
#include "LockQueue.hpp"

int main(int argc, char** argv)
{
    LockQueue<int> bq;

    auto produce = [&](int id) {
        for (int i = 0; i < 100; i++)
            bq.push(id);
    };

    auto consume = [&]() {
        std::optional<int> value;
        for (int i = 0; i < 100; i++)
            value = bq.pop();
    };

    std::vector<std::thread> workers;
    for (int i = 0; i < 20; i++)
    {
        workers.emplace_back(produce, i);
        workers.emplace_back(consume);
    }

    for (auto& w : workers)
        w.join();

    std::printf("queue capacity: %zu\n", bq.capacity());
    std::printf("queue size: %zu\n", bq.size());

    return 0;
}
