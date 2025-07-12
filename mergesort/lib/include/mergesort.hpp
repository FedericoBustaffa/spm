#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <vector>

#include "record.hpp"

void mergesort(std::vector<record>& v);

void merge_blocks(const char* filepath1, const char* filepath2, uint64_t limit);

void mergesort(const char* filepath, uint64_t limit);

#endif
