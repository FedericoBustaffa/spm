#include "serialize.hpp"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "record.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

void dump(const std::vector<record>& records, std::ofstream& file)
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

void dump(const std::vector<record>& records, const char* filepath)
{
    std::ofstream out(filepath, std::ios::binary);
    dump(records, out);
}

std::vector<record> load(std::ifstream& file, uint64_t limit)
{
    std::vector<record> records;
    records.reserve(limit / 20);

    char* buffer = new char[limit];
    size_t index = 0;

    uint64_t key;
    uint32_t length;
    char* payload;

    file.read(buffer, limit);
    size_t bytes = file.gcount();

    while (index < bytes)
    {
        // read the key from the buffer
        if (index + sizeof(uint64_t) > bytes)
        {
            file.seekg(static_cast<std::streamoff>(index - bytes),
                       std::ios::cur);
            break;
        }
        std::memcpy(&key, buffer + index, sizeof(uint64_t));
        index += sizeof(uint64_t);

        // read the length from the buffer
        if (index + sizeof(uint32_t) > bytes)
        {
            file.seekg(
                static_cast<std::streamoff>(index - bytes - sizeof(uint64_t)),
                std::ios::cur);
            break;
        }
        std::memcpy(&length, buffer + index, sizeof(uint32_t));
        index += sizeof(uint32_t);

        // read the payload from the buffer
        if (index + length > bytes)
        {
            file.seekg(static_cast<std::streamoff>(
                           index - bytes - sizeof(uint64_t) - sizeof(uint32_t)),
                       std::ios::cur);
            break;
        }
        payload = new char[length];
        std::memcpy(payload, buffer + index, length);
        index += length;

        records.emplace_back(key, length, payload);
    }

    delete[] buffer;

    return records;
}

std::vector<record> load(const char* filepath, uint64_t limit)
{
    std::ifstream file(filepath, std::ios::binary);
    limit = limit == 0 ? fs::file_size(filepath) : limit;
    return load(file, limit);
}
