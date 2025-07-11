#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <vector>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n);

void dump(const record& r, std::ofstream& file);

void dump(const std::vector<record>& records, std::ofstream& file);

record load(std::ifstream& file);

std::vector<record> load(std::ifstream& file, uint64_t limit);

#endif
