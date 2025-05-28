#include <iostream>
#include <vector>

#include "benchmarks.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
<<<<<<< HEAD
    int64_t e = 20;
=======
    int64_t e = 15;

    if (argc >= 2)
        e = std::atol(argv[1]);

>>>>>>> refs/remotes/origin/master
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

    Timer timer;
    timer.start();
    std::vector<int> s_res = sequential(numbers);
    double stime = timer.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    thread_pool pool(w, q);
    std::cout << "thread pool with " << w << " workers and " << q
              << " queue slots" << std::endl;

    timer.start();
    std::vector<int> p_res = submit(numbers, pool);
    double ptime = timer.stop();
    std::cout << "submit time: " << ptime << std::endl;

    timer.start();
    std::vector<int> pa_res = submit_async(numbers, pool);
    double patime = timer.stop();
    std::cout << "submit async time: " << patime << std::endl;

    std::cout << "submit speedup: " << (stime / ptime) << std::endl;
    std::cout << "submit async speedup: " << (stime / patime) << std::endl;

    auto compare = [](const std::vector<int>& a, const std::vector<int>& b) {
        bool error = false;
        for (size_t i = 0; i < a.size(); i++)
        {
            if (a[i] != b[i])
            {
                error = true;
                std::cout << "a[" << i << "] = " << a[i] << " | b[" << i
                          << "] = " << b[i] << std::endl;
            }
        }

        return error;
    };

    std::cout << pool.queue_capacity() << std::endl;

    if (!compare(s_res, p_res))
        std::cout << "submit no errors" << std::endl;

    if (s_res.size() == pa_res.size())
    {
        if (!compare(s_res, pa_res))
            std::cout << "submit async no errors" << std::endl;
    }

    std::cout << "dynamic speed up: " << (stime / dynamic_time) << std::endl;

    return 0;
}
