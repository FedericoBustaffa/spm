#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <fstream>

#include "record.hpp"

void dump(const std::vector<record>& records, std::ofstream& file);

void dump(const std::vector<record>& records, const char* filepath);

std::vector<record> load(std::ifstream& file, uint64_t limit = 0);

std::vector<record> load(const char* filepath, uint64_t limit = 0);

#endif
