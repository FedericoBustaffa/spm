#include <cstdio>
#include <omp.h>
#include <thread>

#include "timer.hpp"

using namespace std::chrono_literals;

int main()
{
    spm::timer timer;
    timer.start();
    for (int i = 0; i < 5; i++)
        std::this_thread::sleep_for(1s);
    std::printf("%.6f\n", timer.stop());

    timer.start();
#pragma omp parallel for
    for (int i = 0; i < 5; i++)
        std::this_thread::sleep_for(1s);

    std::printf("%.6f\n", timer.stop());
}
