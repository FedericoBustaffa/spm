#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n, uint64_t max_payload);

uint64_t mem_usage(const std::vector<record>& v);

uint64_t parse_mem_limit(const char* limit);

#endif
