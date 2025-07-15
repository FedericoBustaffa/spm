#include "utils.hpp"

#include <algorithm>
#include <cstdint>
#include <random>
#include <regex>

#include "record.hpp"

static std::mt19937 generator(42);

std::vector<record> generate_records(uint64_t n, uint64_t max_payload)
{
    std::uniform_int_distribution<uint32_t> length_dist(8, max_payload);
    std::uniform_int_distribution<char> payload_dist(97, 122);

    std::vector<record> records;
    records.reserve(n);

    uint32_t length;
    char* payload;

    for (uint64_t i = 0; i < n; i++)
    {
        length = length_dist(generator);
        // length = max_payload; // TODO: only for debugging -> should be random
        payload = new char[length];
        for (uint32_t i = 0; i < length; i++)
            payload[i] = payload_dist(generator);

        records.emplace_back(i, length, payload);
    }

    std::shuffle(records.begin(), records.end(), generator);

    return records;
}

uint64_t mem_usage(const std::vector<record>& v)
{
    uint64_t bytes = 0;
    for (const auto& i : v)
        bytes += sizeof(uint64_t) + sizeof(uint32_t) + i.length();

    return bytes;
}

uint64_t parse_mem_limit(const char* limit)
{
    std::string input = std::move(limit);
    std::regex pattern(R"(^([1-9][0-9]*)(B|KB|MB|GB)$)");
    std::smatch matches;

    uint64_t bytes;

    if (std::regex_search(input, matches, pattern))
    {
        if (matches[2] == "B")
            bytes = std::stoull(matches[1]);
        else if (matches[2] == "KB")
            bytes = std::stoull(matches[1]) * 1024;
        else if (matches[2] == "MB")
            bytes = std::stoull(matches[1]) * 1024 * 1024;
        else
            bytes = std::stoull(matches[1]) * 1024 * 1024 * 1024;
    }
    else
        return 0;

    std::printf("%s = %lu bytes\n", limit, bytes);

    return bytes;
}
