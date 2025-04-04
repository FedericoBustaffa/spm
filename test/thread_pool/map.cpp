#include <cassert>
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

double sum(double a, double b)
{
    std::this_thread::sleep_for(0.5ms);
    return a + b;
}

template <typename T, typename Func, typename... Args>
std::vector<T> map(Func &&func, const std::vector<T> &v)
{
    std::vector<T> res;
    res.reserve(v.size());

    for (const auto &i : v)
        res.push_back(func(i));

    return res;
}

int main(int argc, const char **argv)
{
    size_t n = 1000;
    size_t chunksize = n / std::thread::hardware_concurrency();
    if (argc >= 2)
        n = std::atol(argv[1]);

    if (argc >= 3)
        chunksize = std::atol(argv[2]);

    timer t;
    std::vector<double> v = generate_vector(n);

    auto partial_sum = std::bind(sum, 10.0, std::placeholders::_1);

    t.start();
    auto v1 = map(partial_sum, v);
    double stime = t.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    thread_pool pool;
    t.start();
    auto v2 = pool.map(partial_sum, v, chunksize);

    double ptime = t.stop();
    std::cout << "parallel time: " << ptime << " seconds" << std::endl;
    std::cout << "speed up: " << stime / ptime << std::endl;

    assert(std::equal(v1.begin(), v1.end(), v2.begin()));

    return 0;
}
