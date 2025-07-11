#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "record.hpp"

#define MAX_PAYLOAD 64

std::vector<record> generate_records(uint64_t n);

#endif
