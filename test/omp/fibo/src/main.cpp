#include <cstdio>
#include <string>

#include <omp.h>

#include "timer.hpp"

int fibo(int n)
{
    if (n < 2)
        return n;

    int first, second;

#pragma omp task shared(first)
    first = fibo(n - 1);

#pragma omp task shared(second)
    second = fibo(n - 2);

#pragma omp taskwait
    return first + second;
}

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::printf("USAGE: %s <N>\n", argv[0]);
        return 1;
    }

    int n = std::stoi(argv[1]);

    spm::timer timer;
    timer.start();

    int nth;

#pragma omp parallel
    {
#pragma omp single
        nth = fibo(n);
    }

    std::printf("fibo(%d) = %d computed in %.4f s with %d threads\n", n, nth,
                timer.stop(), omp_get_max_threads());

    return 0;
}
