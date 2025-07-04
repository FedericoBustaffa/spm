#include <cstdint>
#include <cstdio>
#include <string>

#include <ff/parallel_for.hpp>

#include "timer.hpp"

bool is_prime(uint64_t n)
{
    if (n < 2)
        return false;

    for (uint64_t i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }

    return true;
}

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <exp> <nworkers>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoull(argv[1]);
    int nworkers = std::stoi(argv[2]);

    spm::timer timer;
    timer.start();

    uint64_t counter = 0;
    ff::parallel_reduce(
        counter, 0, 2, n, 1, 1,
        [&](long i) { counter += (uint64_t)is_prime(i); }, []() {}, nworkers);

    // std::printf("found %lu primes in %.4f seconds with %d workers\n",
    // counter,
    //             timer.stop(), nworkers);

    return 0;
}
