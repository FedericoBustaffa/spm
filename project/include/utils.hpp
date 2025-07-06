#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n);

void serialize(const std::vector<record>& records);

std::vector<record> deserialize(const char* filepath);

#endif
