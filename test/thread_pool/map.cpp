#include <cassert>
#include <iostream>
#include <random>
#include <vector>

#include "thread_pool.hpp"
#include "timer.hpp"

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
    // add random work
    generate_vector(500000);
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
    size_t vec_size = 1000;
    size_t chunksize = vec_size / std::thread::hardware_concurrency();
    size_t nworkers = 0;
    size_t queue_capacity = 0;

    if (argc >= 2)
        vec_size = std::atol(argv[1]);

    if (argc >= 3)
        chunksize = std::atol(argv[2]);

    if (argc >= 4)
        nworkers = std::atol(argv[3]);

    if (argc >= 5)
        queue_capacity = std::atol(argv[4]);

    timer t;
    std::vector<double> v = generate_vector(vec_size);

    auto partial_sum = std::bind(sum, 10.0, std::placeholders::_1);

    t.start();
    auto v1 = map(partial_sum, v);
    double stime = t.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    thread_pool pool(nworkers, queue_capacity);
    t.start();
    auto v2 = pool.map(partial_sum, v, chunksize);
    double ptime = t.stop();
    std::cout << "parallel time: " << ptime << " seconds" << std::endl;

    std::cout << "speed up: " << stime / ptime << std::endl;

    assert(std::equal(v1.begin(), v1.end(), v2.begin()));

    return 0;
}
