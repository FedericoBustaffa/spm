#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <vector>

#include "record.hpp"

// --- internal.cpp ---
void mergesort(std::vector<record>& v);

// --- external.cpp ---
void mergesort(const char* filepath, uint64_t limit);

#endif
