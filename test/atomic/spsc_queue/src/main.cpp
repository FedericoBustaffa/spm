#include <cstdio>

#include "atomic_queue.hpp"
#include "thread_pool.hpp"

int main(int argc, char** argv)
{
    thread_pool tp(8);

    atomic_queue<int> queue(10);
    auto produce = [&]() {
        for (size_t i = 0; i < (1 << 15); i++)
            queue.push(i);
    };

    auto consume = [&]() {
        for (size_t i = 0; i < (1 << 15); i++)
            queue.pop();
    };

    for (size_t i = 0; i < 20; i++)
        tp.submit(produce);

    for (size_t i = 0; i < 20; i++)
        tp.submit(consume);

    return 0;
}
