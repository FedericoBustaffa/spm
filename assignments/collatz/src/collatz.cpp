#include "collatz.hpp"

uint64_t collatz_steps(uint64_t n)
{
    uint64_t steps = 0;
    while (n != 1)
    {
        n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
        ++steps;
    }

    return steps;
}
