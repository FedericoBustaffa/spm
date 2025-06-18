#ifndef COLLATZ_HPP
#define COLLATZ_HPP

#include <cstdint>
#include <vector>

uint64_t collatz_steps(uint64_t n);

double sequential(const std::vector<std::pair<uint64_t, uint64_t>>& ranges);
double static_schedule(
    size_t workers, const std::vector<std::pair<uint64_t, uint64_t>>& ranges);
double dynamic_schedule(
    size_t workers, const std::vector<std::pair<uint64_t, uint64_t>>& ranges);

#endif
