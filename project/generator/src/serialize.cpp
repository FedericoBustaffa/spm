#include "serialize.hpp"

#include <cstdint>
#include <fstream>

#include "Record.hpp"

void serialize(const std::vector<Record>& records, const char* filepath)
{
    uint64_t total_bytes = 0;
    uint64_t payload_bytes = 0;

    uint64_t key;
    uint32_t length;
    const char* payload;

    std::ofstream out(filepath, std::ios::binary);
    for (const auto& r : records)
    {
        key = r.key();
        length = r.length();
        payload = r.payload();

        out.write(reinterpret_cast<const char*>(&key), sizeof(uint64_t));
        out.write(reinterpret_cast<const char*>(&length), sizeof(uint32_t));
        out.write(payload, length);

        // update bytes generated
        payload_bytes += r.length();
        total_bytes += sizeof(uint64_t) + sizeof(uint32_t) + r.length();
    }

    std::printf("payload generated: %lu bytes\n", payload_bytes);
    std::printf("total generated: %lu bytes\n", total_bytes);
}

std::vector<Record> deserialize(const char* filepath)
{
    std::vector<Record> records;

    uint64_t total_bytes = 0;
    uint64_t payload_bytes = 0;

    uint64_t key;
    uint32_t length;
    char* payload = new char[MAX_PAYLOAD];

    std::ifstream in(filepath, std::ios::binary);
    while (!in.eof())
    {
        in.read(reinterpret_cast<char*>(&key), sizeof(uint64_t));
        in.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));
        in.read(payload, length);

        if (in.eof())
            break;

        records.emplace_back(key, length, payload);

        // update bytes for stats
        payload_bytes += length;
        total_bytes += sizeof(uint64_t) + sizeof(uint32_t) + length;
    }

    std::printf("payload read: %lu bytes\n", payload_bytes);
    std::printf("total read: %lu bytes\n", total_bytes);

    delete[] payload;

    return records;
}
