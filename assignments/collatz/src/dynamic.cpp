#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "collatz.hpp"

double dynamic(size_t workers_num,
               const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    std::atomic<uint64_t>* steps = new std::atomic<uint64_t>[ranges.size()];

    std::vector<std::thread> workers;
    workers.reserve(workers_num);

    std::queue<std::pair<size_t, uint64_t>> buffer;
    std::mutex mtx;
    std::condition_variable empty;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < workers_num; i++)
    {
        workers.emplace_back(
            [&](size_t id) {
                std::pair<size_t, uint64_t> idx_value;
                while (true)
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    while (buffer.size() == 0)
                        empty.wait(lock);

                    idx_value = std::move(buffer.front());
                    buffer.pop();
                    lock.unlock();

                    if (idx_value.second == 0)
                        break;

                    steps[idx_value.first] += collatz_steps(idx_value.second);
                }
            },
            i);
    }

    for (size_t i = 0; i < ranges.size(); i++)
    {
        for (uint64_t j = ranges[i].first; j <= ranges[i].second; j++)
        {
            std::unique_lock<std::mutex> lock(mtx);
            buffer.push({i, j});
            lock.unlock();
            empty.notify_one();
        }
    }

    for (size_t i = 0; i < workers_num; i++)
    {
        std::unique_lock<std::mutex> lock(mtx);
        buffer.push({0, 0}); // termination value
        lock.unlock();
        empty.notify_one();
    }

    for (auto& w : workers)
        w.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    for (size_t i = 0; i < ranges.size(); i++)
    {
        std::printf("%lu-%lu: %lu steps\n", ranges[i].first, ranges[i].second,
                    steps[i].load());
    }

    delete[] steps;

    return duration.count();
}

class AtomicQueue
{
public:
    AtomicQueue(size_t capacity) : m_Capacity(capacity), m_Size(0) {}

    inline size_t capacity() const { return m_Capacity; }

    inline size_t size() const { return m_Size; }

    void push(std::pair<size_t, uint64_t>&& idx_value) {}

    std::pair<size_t, uint64_t> pop() { return {}; }

    ~AtomicQueue() {}

private:
    size_t m_Capacity;
    size_t m_Size;
};

double dynamic_atomic(size_t workers_num,
                      const std::vector<std::pair<uint64_t, uint64_t>>& ranges)
{
    std::atomic<uint64_t>* steps = new std::atomic<uint64_t>[ranges.size()];

    std::vector<std::thread> workers;
    workers.reserve(workers_num);

    AtomicQueue buffer(8192);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < workers_num; i++)
    {
        workers.emplace_back(
            [&](size_t id) {
                std::pair<size_t, uint64_t> idx_value;
                while (true)
                {

                    idx_value = buffer.pop();
                    if (idx_value.second == 0)
                        break;

                    steps[idx_value.first] += collatz_steps(idx_value.second);
                }
            },
            i);
    }

    for (size_t i = 0; i < ranges.size(); i++)
    {
        for (uint64_t j = ranges[i].first; j <= ranges[i].second; j++)
            buffer.push({i, j});
    }

    for (size_t i = 0; i < workers_num; i++)
        buffer.push({0, 0}); // termination value

    for (auto& w : workers)
        w.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    for (size_t i = 0; i < ranges.size(); i++)
    {
        std::printf("%lu-%lu: %lu steps\n", ranges[i].first, ranges[i].second,
                    steps[i].load());
    }

    delete[] steps;

    return duration.count();
}
