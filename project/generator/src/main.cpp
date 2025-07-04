#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include "Record.hpp"

#define MAX_PAYLOAD 256

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    uint64_t n = std::stoul(argv[1]);
    std::printf("generating %lu records\n", n);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint32_t> distribution(8, MAX_PAYLOAD);

    std::vector<Record> records;
    for (uint64_t i = 0; i < n; i++)
        records.emplace_back(distribution(generator));

    std::ofstream file("file.dat", std::ios::binary);
    for (const auto& r : records)
        file << r.key() << r.length() << r.payload() << std::endl;

    uint64_t total = 0;
    uint64_t payload = 0;
    for (const auto& r : records)
    {
        payload += r.length();
        total += sizeof(uint64_t) + sizeof(uint32_t) + r.length();
    }

    std::printf("payload generated: %lu bytes\n", payload);
    std::printf("total generated: %lu bytes\n", total);

    return 0;
}
