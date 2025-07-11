#include "serialize.hpp"

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

void serialize(const std::vector<record>& records, const char* filepath)
{
    std::ofstream out(filepath, std::ios::binary);
    serialize(records, out);
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

std::vector<record> deserialize(std::ifstream& file, uint64_t limit)
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
        if (bytes + sizeof(uint64_t) + sizeof(uint32_t) + 8 > limit)
            break;

        // read one record
        temp = load(file);
        bytes += sizeof(uint64_t) + sizeof(uint32_t) + temp.length();

        // invalid record means EOF
        if (!temp.is_valid())
            break;

        // if adding the record exceeds the limit, the file cursor position is
        // put back at the start of the record that will be read next iteration
        if (bytes > limit)
        {
            file.seekg(pos);
            break;
        }

        records.push_back(std::move(temp));
    }

    return records;
}
