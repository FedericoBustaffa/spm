#include <cstdio>

#include <omp.h>

int main()
{
#pragma omp parallel
    { // <- spawning of threads
        int i = omp_get_thread_num();
        int n = omp_get_num_threads();
        std::printf("Hello from thread %d of %d\n", i, n);
    } // <- joining of threads

    return 0;
}
