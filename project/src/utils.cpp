#include "mergesort.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <random>
#include <sstream>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n)
{
    std::mt19937 generator(42);
    std::uniform_int_distribution<uint32_t> distribution(8, MAX_PAYLOAD);

    std::vector<record> records;
    for (uint64_t i = 0; i < n; i++)
        records.emplace_back(distribution(generator));

    std::shuffle(records.begin(), records.end(), generator);

    return records;
}

const char* serialize(const std::vector<record>& records)
{
    uint64_t total_bytes = 0;
    uint64_t payload_bytes = 0;

    uint64_t key;
    uint32_t length;
    const char* payload;

    // file name
    std::stringstream ss;
    ss << "records_" << records.size() << ".dat";

    std::ofstream out(ss.str(), std::ios::binary);
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

    std::printf("file produced: %s\n", ss.str().c_str());
    std::printf("payload generated: %lu bytes\n", payload_bytes);
    std::printf("total generated: %lu bytes\n", total_bytes);

    return std::move(ss.str().c_str());
}

std::vector<record> deserialize(const char* filepath)
{
    std::vector<record> records;

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
