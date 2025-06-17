#ifndef COLLATZ_HPP
#define COLLATZ_HPP

#include <cstdint>

uint64_t collatz_steps(uint64_t n);

double sequential();
double static_schedule();
double dynamic_schedule();

#endif
