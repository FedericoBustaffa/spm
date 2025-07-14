#include "serialize.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>

#include "record.hpp"
#include "utils.hpp"

void dump_record(const record& r, std::ofstream& file)
{
    file.write(reinterpret_cast<const char*>(&r.key()), sizeof(uint64_t));
    file.write(reinterpret_cast<const char*>(&r.length()), sizeof(uint32_t));
    file.write(r.payload(), r.length());
}

void dump_vector(const std::vector<record>& records, std::ofstream& file)
{
    uint64_t bufsize = mem_usage(records);
    char* buffer = new char[bufsize];
    size_t index = 0;

    for (const auto& r : records)
    {
        std::memcpy(buffer + index, &r.key(), sizeof(uint64_t));
        index += sizeof(uint64_t);

        std::memcpy(buffer + index, &r.length(), sizeof(uint32_t));
        index += sizeof(uint32_t);

        std::memcpy(buffer + index, r.payload(), r.length());
        index += r.length();
    }

    file.write(buffer, bufsize);
    delete[] buffer;
}

void dump_vector(const std::vector<record>& records, const char* filepath)
{
    std::ofstream out(filepath, std::ios::binary);
    dump_vector(records, out);
}

record load_record(std::ifstream& file)
{
    uint64_t key;
    file.read(reinterpret_cast<char*>(&key), sizeof(uint64_t));
    if (file.eof())
        return record();

    uint32_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));

    char* payload = new char[length];
    file.read(payload, length);

    return record(key, length, payload);
}

std::vector<record> load_vector(std::ifstream& file, uint64_t limit)
{
    std::vector<record> records;

    // try to optimize reallocation in the worst case (many small records)
    records.reserve(limit / 20); // 20 is the minimum size for a record

    record temp;
    uint64_t bytes = 0;

    while (true)
    {
        // get the current file cursor position
        int pos = file.tellg();

        // if reading the key, the length and a minimum payload increase the
        // memory usage beyond the limit don't even try to read the next record
        if (bytes + sizeof(uint64_t) + sizeof(uint32_t) + 8 > limit &&
            limit > 0)
            break;

        // read one record
        temp = load_record(file);
        bytes += sizeof(uint64_t) + sizeof(uint32_t) + temp.length();

        // invalid record means EOF
        if (!temp.is_valid())
            break;

        // if adding the record exceeds the limit, the file cursor position is
        // put back at the start of the record that will be read next iteration
        if (bytes > limit && limit > 0)
        {
            file.seekg(pos);
            break;
        }

        records.push_back(std::move(temp));
    }

    return records;
}

std::vector<record> load_vector(const char* filepath, uint64_t limit)
{
    std::ifstream file(filepath, std::ios::binary);
    return load_vector(file, limit);
}
