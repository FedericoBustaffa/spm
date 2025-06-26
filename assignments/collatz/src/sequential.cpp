#include "collatz.hpp"

#include "timer.hpp"

double sequential(const range& range)
{
    uint64_t counter = 0;
    spm::timer timer;
    timer.start();
    for (uint64_t i = range.a; i <= range.b; i++)
        counter += collatz_steps(i);
    double time = timer.stop();

    return time;
}
