#include "mergesort.hpp"

#include <cstdint>
#include <fstream>

#include "record.hpp"

void dump(const record& r, std::ofstream& file)
{
    file.write(reinterpret_cast<const char*>(&r.key()), sizeof(uint64_t));
    file.write(reinterpret_cast<const char*>(&r.length()), sizeof(uint32_t));
    file.write(r.payload(), r.length());
}

void serialize(const std::vector<record>& records, std::ofstream& file)
{
    for (const auto& r : records)
        dump(r, file);
}

record load(std::ifstream& file)
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

std::vector<record> deserialize(std::ifstream& file)
{
    std::vector<record> records;
    record temp;

    while (true)
    {
        temp = load(file);
        if (!temp.is_valid())
            break;

        records.push_back(std::move(temp));
    }

    return records;
}
