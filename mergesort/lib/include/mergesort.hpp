#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <vector>

#include "record.hpp"

void mergesort(std::vector<record>& v);

void mergesort(const char* filepath, uint64_t limit);

#endif
