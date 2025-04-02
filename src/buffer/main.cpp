#include <iostream>
#include <thread>
#include <vector>

#include "buffer.hpp"

#define N 100

void produce(buffer<int> *b)
{
    for (int i = 0; i < N; i++)
    {
        std::cout << "produced: " << i << " from " << std::this_thread::get_id() << std::endl;
        b->push(i);
    }
}

void consume(buffer<int> *b)
{
    for (int i = 0; i < N; i++)
    {
        std::cout << "consumed: " << i << " from " << std::this_thread::get_id() << std::endl;
        b->pop();
    }
}

int main(int argc, const char **argv)
{
    std::vector<std::thread> producers, consumers;
    buffer<int> b(1);

    for (int i = 0; i < 10; i++)
    {
        producers.emplace_back(produce, &b);
        consumers.emplace_back(consume, &b);
    }

    for (int i = 0; i < 10; i++)
    {
        producers[i].join();
        consumers[i].join();
    }

    std::cout << b.size() << std::endl;

    return 0;
}
