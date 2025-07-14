#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <fstream>

#include "record.hpp"

void dump_record(const record& r, std::ofstream& file);

void dump_vector(const std::vector<record>& records, std::ofstream& file);

void dump_vector(const std::vector<record>& records, const char* filepath);

record load_record(std::ifstream& file);

std::vector<record> load_vector(std::ifstream& file, uint64_t limit = 0);

std::vector<record> load_vector(const char* filepath, uint64_t limit = 0);

#endif
