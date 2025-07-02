#include <cstdint>
#include <cstdio>
#include <string>

#include <omp.h>

#include "timer.hpp"

bool is_prime(uint64_t n)
{
    if (n < 2)
        return false;

    for (uint64_t i = 2; i * i <= n; i++)
        if (n % i == 0)
            return false;

    return true;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <exp>\n", argv[0]);
        return 1;
    }

    uint64_t n = 1ULL << std::stoull(argv[1]);

    uint64_t primes = 0;

    spm::timer timer;
    timer.start();

#pragma omp parallel for reduction(+ : primes)
    for (uint64_t i = 2; i <= n; i++)
        primes += is_prime(i);

    double time = timer.stop();
    std::printf("%d worker(s) found %lu primes in %.4f s\n",
                omp_get_max_threads(), primes, time);

    return 0;
}
