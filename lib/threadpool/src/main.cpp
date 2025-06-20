#include <iostream>
#include <vector>

#include "benchmarks.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
    int64_t e = 10;
    int64_t n = 1 << e; // # of numbers
    if (argc >= 2)
    {
        e = std::atol(argv[1]);
        n = 1 << e;
    }

    int64_t w = 0; // # of workers
    if (argc >= 3)
        w = std::atol(argv[2]);

    int64_t q = 0; // # task queue slots
    if (argc >= 4)
        q = std::atol(argv[3]);

    /*
     * Every test compute the fibonacci number of all the n numbers contained
     * in the std::vector "numbers"
     */
    std::vector<int> numbers = generate_numbers(n);

    spm::threadpool pool(w, q);

    std::cout << "simulation on " << n << " numbers and a ";
    std::cout << "thread pool with " << pool.size() << " workers and ";
    if (q > 0)
        std::cout << q << " queue slots" << std::endl;
    else
        std::cout << "unbounded queue" << std::endl;

    spm::timer timer;
    timer.start();
    std::vector<int> s_res = sequential(numbers);
    double stime = timer.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    timer.start();
    std::vector<int> p_res = submit(numbers, pool);
    double ptime = timer.stop();
    std::cout << "submit time: " << ptime << std::endl;

    auto compare = [](const std::vector<int>& a, const std::vector<int>& b) {
        bool error = false;
        if (a.size() != b.size())
        {
            std::cout << " vectors with different sizes" << std::endl;
            return true;
        }

        for (size_t i = 0; i < a.size(); i++)
        {
            if (a[i] != b[i])
            {
                error = true;
                std::cout << " a[" << i << "] = " << a[i] << " | b[" << i
                          << "] = " << b[i] << std::endl;
            }
        }

        return error;
    };

    std::cout << "submit speedup: " << (stime / ptime) << std::flush;
    if (!compare(s_res, p_res))
        std::cout << " no errors" << std::endl;

    return 0;
}
