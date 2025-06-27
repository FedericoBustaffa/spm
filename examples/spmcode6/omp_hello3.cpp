#include <cstdio>
#include <omp.h>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    using namespace std::chrono_literals;

    auto F = [](int i) {
        std::this_thread::sleep_for(100ms);
        std::printf("Hello from thread %d\n", i);
    };
    int nth = (argc > 1) ? std::stol(argv[1]) : 1;

    double start = omp_get_wtime();

#pragma omp parallel if (nth >= 5) num_threads(nth)
    F(omp_get_thread_num());

    double stop = omp_get_wtime();

    std::printf("Time %.2f (ms)\n", (stop - start) * 1000.0);

    return 0;
}
