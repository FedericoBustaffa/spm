#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n);

const char* serialize(const std::vector<record>& records);

std::vector<record> deserialize(const char* filepath);

void mergesort(std::vector<record>& v);

void mergesort(const char* filepath);

#endif
