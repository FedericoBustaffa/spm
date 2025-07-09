#include "utils.hpp"

#include <algorithm>
#include <cstdint>
#include <random>

#include "record.hpp"

std::vector<record> generate_records(uint64_t n)
{
    std::vector<record> records;
    records.reserve(n);

    std::mt19937 generator(42);
    std::uniform_int_distribution<uint32_t> length_dist(8, MAX_PAYLOAD);
    std::uniform_int_distribution<char> payload_dist(97, 122);

    uint32_t length;
    char* payload;

    for (uint64_t i = 0; i < n; i++)
    {
        length = length_dist(generator);
        payload = new char[length];
        for (uint32_t i = 0; i < length; i++)
            payload[i] = payload_dist(generator);

        records.emplace_back(i, length, payload);
    }

    std::shuffle(records.begin(), records.end(), generator);

    return records;
}
