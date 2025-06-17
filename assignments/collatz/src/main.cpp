#include <cstdio>

#include "collatz.hpp"

int main(int argc, const char**)
{
    double stime = sequential();
    std::printf("sequential time: %.4f ns\n", stime);

    double static_time = static_schedule();
    std::printf("sequential time: %.4f ns\n", static_time);

    double dynamic_time = dynamic_schedule();
    std::printf("sequential time: %.4f ns\n", dynamic_time);

    std::printf("static schedule speedup: %.2f\n", (stime / static_time));
    std::printf("dynamic schedule speedup: %.2f\n", (stime / dynamic_time));

    return 0;
}
