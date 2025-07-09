#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <fstream>

#include "record.hpp"

void dump(const record& r, std::ofstream& file);

void serialize(const std::vector<record>& records, std::ofstream& file);

record load(std::ifstream& file);

std::vector<record> deserialize(std::ifstream& file, uint64_t limit);

#endif
