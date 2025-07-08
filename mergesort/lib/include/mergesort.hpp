#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <fstream>
#include <vector>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n);

void serialize(const std::vector<record>& records, std::ofstream& file);

std::vector<record> deserialize(std::ifstream& file);

void mergesort(std::vector<record>& v);

// void mergesort(const char* filepath);

#endif
