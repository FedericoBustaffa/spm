#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <vector>

#include "Record.hpp"

void serialize(const std::vector<Record>& records, const char* filepath);

std::vector<Record> deserialize(const char* filepath);

#endif
