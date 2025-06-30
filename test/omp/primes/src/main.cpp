#include <cstdint>
#include <cstdio>
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

    spm::timer timer;
    timer.start();

    uint64_t primes = 0;

#pragma omp parallel for reduction(+ : primes) schedule(dynamic)
    for (uint64_t i = 2; i <= n; i++)
        primes += is_prime(i);

    double time = timer.stop();
    std::printf("%d workers time: %.6f\n", omp_get_max_threads(), time);

    // std::printf("primes: %lu\n", primes);

    return 0;
}
