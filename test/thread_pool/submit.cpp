#include <algorithm>
#include <future>
#include <iostream>
#include <random>
#include <vector>

#include "thread_pool.hpp"
#include "timer.hpp"

using namespace std::chrono_literals;

std::vector<double> generate_vector(size_t n_elems)
{
    std::mt19937 rand_eng(42);
    std::normal_distribution<double> dist;
    std::vector<double> values;

    for (size_t i = 0; i < n_elems; i++)
        values.push_back(dist(rand_eng));

    return values;
}

void sequential_generation(size_t n, size_t m)
{
    for (size_t i = 0; i < n; i++)
        auto v = generate_vector(m);
}

void parallel_generation(size_t n, size_t m)
{
    thread_pool pool;
    std::vector<std::future<std::vector<double>>> results;
    results.reserve(n);

    for (size_t i = 0; i < n; i++)
        results.push_back(pool.submit(generate_vector, m));

    for (auto &r : results)
        auto v = r.get();
}

void parallel_generation_async(size_t n, size_t m)
{
    thread_pool pool;
    std::vector<std::future<std::vector<double>>> results;
    results.reserve(n);

    for (size_t i = 0; i < n; i++)
        results.push_back(pool.submit_async(generate_vector, m));

    for (auto &r : results)
        auto v = r.get();
}

void dummy_test(size_t n)
{
    thread_pool pool(4, 1);
    std::vector<std::future<int>> results;
    results.reserve(n);

    for (size_t i = 0; i < n; i++)
    {
        results.push_back(pool.submit([]() {
            std::this_thread::sleep_for(3s);
            return 0;
        }));
        std::cout << i << " submitted" << std::endl;
    }

    for (size_t i = 0; i < results.size(); i++)
        results[i].get();
}

int main(int argc, const char **argv)
{
    size_t n = 10000;
    size_t m = 10000;

    if (argc >= 2)
        n = std::atol(argv[1]);

    if (argc >= 3)
        m = std::atol(argv[2]);

    timer t;

    t.start();
    sequential_generation(n, m);
    double stime = t.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    t.start();
    parallel_generation(n, m);
    double ptime = t.stop();
    std::cout << "parallel time: " << ptime << " seconds" << std::endl;

    t.start();
    parallel_generation_async(n, m);
    double ptime_async = t.stop();
    std::cout << "parallel async time: " << ptime_async << " seconds"
              << std::endl;

    std::cout << "speed up: " << stime / ptime << std::endl;
    std::cout << "speed up async: " << stime / ptime_async << std::endl;

    // to test the correct submission of tasks for bounded queue
    // dummy_test(n);

    return 0;
}
