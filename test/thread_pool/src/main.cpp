#include <iostream>
#include <vector>

#include "benchmarks.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
    int64_t e = 20;
    int64_t n = 1 << e;
    if (argc >= 2)
    {
        e = std::atol(argv[1]);
        n = 1 << e;
    }

    int64_t w = 0;
    if (argc >= 3)
        w = std::atol(argv[2]);

    int64_t q = 0;
    if (argc >= 4)
        q = std::atol(argv[3]);

    std::vector<int> numbers = generate_numbers(n);

    thread_pool pool(w, q);
    std::cout << "thread pool with " << w << " workers and " << q
              << " queue slots" << std::endl;

    Timer timer;
    timer.start();
    std::vector<int> s_res = sequential(numbers);
    double stime = timer.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    timer.start();
    std::vector<int> p_res = submit(numbers, pool);
    double ptime = timer.stop();
    std::cout << "submit time: " << ptime << std::endl;

    timer.start();
    std::vector<int> pa_res = submit_async(numbers, pool);
    double patime = timer.stop();
    std::cout << "submit async time: " << patime << std::endl;

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

    std::cout << "submit async speedup: " << (stime / patime) << std::flush;
    if (!compare(s_res, pa_res))
        std::cout << " no errors" << std::endl;

    return 0;
}
