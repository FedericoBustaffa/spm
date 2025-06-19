#ifndef COLLATZ_HPP
#define COLLATZ_HPP

#include <cstdint>
#include <vector>

uint64_t collatz_steps(uint64_t n);

double sequential(const std::vector<std::pair<uint64_t, uint64_t>>& ranges);

double block(size_t workers_num,
             const std::vector<std::pair<uint64_t, uint64_t>>& ranges);

double cyclic(size_t workers_num,
              const std::vector<std::pair<uint64_t, uint64_t>>& ranges);

double dynamic(size_t workers_num,
               const std::vector<std::pair<uint64_t, uint64_t>>& ranges);

#endif
