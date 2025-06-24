#include "collatz.hpp"

uint64_t sequential(const range& range)
{
    uint64_t counter = 0;
    for (uint64_t i = range.a; i <= range.b; i++)
        counter += collatz_steps(i);

    return counter;
}
