#include <iostream>
#include <vector>

#include "threadpool.hpp"
#include "timer.hpp"

std::vector<int> generate_numbers(size_t n, int min = 25, int max = 30);
std::vector<int> sequential(const std::vector<int>& numbers);
std::vector<int> submit(const std::vector<int>& numbers, spm::threadpool& pool);

int main(int argc, const char** argv)
{
    // # of numbers
    int64_t e = 10;
    int64_t n = 1 << e;
    if (argc >= 2)
    {
        e = std::atol(argv[1]);
        n = 1 << e;
    }

    // # of workers
    int64_t w = 0;
    if (argc >= 3)
        w = std::atol(argv[2]);

    // # task queue slots
    int64_t q = 1 << 10UL;
    if (argc >= 4)
        q = 1 << std::atol(argv[3]);

    // Every test compute the fibonacci number of all the n numbers contained
    // in the std::vector "numbers"
    std::vector<int> numbers = generate_numbers(n);
    spm::threadpool pool(w, q);

    std::cout << "simulation stats" << std::endl;
    std::cout << n << " fibonacci numbers" << std::endl;
    std::cout << pool.size() << " workers" << std::endl;
    std::cout << q << " queue slots" << std::endl;
    std::cout << "**********************" << std::endl;

    spm::timer timer;
    timer.start();
    std::vector<int> s_res = sequential(numbers);
    double stime = timer.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    timer.start();
    std::vector<int> p_res = submit(numbers, pool);
    double ptime = timer.stop();
    std::cout << "submit time: " << ptime << std::endl;

    std::cout << "submit speedup: " << (stime / ptime) << std::endl;

    bool error = false;
    if (s_res.size() != p_res.size())
    {
        std::cout << " vectors with different sizes" << std::endl;
        error = true;
    }

    for (size_t i = 0; i < s_res.size(); i++)
    {
        if (s_res[i] != p_res[i])
        {
            error = true;
            std::cout << " a[" << i << "] = " << s_res[i] << " | b[" << i
                      << "] = " << p_res[i] << std::endl;
        }
    }

    if (!error)
        std::cout << "no errors occurred" << std::endl;

    return 0;
}
