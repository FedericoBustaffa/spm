#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "Record.hpp"
#include "serialize.hpp"

#define MAX_PAYLOAD 8

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n> [print=0]\n", argv[0]);
        return 1;
    }

    uint64_t n = std::stoul(argv[1]);
    std::printf("generating %lu records\n", n);
    std::printf("\n");

    bool print = false;
    if (argc == 3)
        print = (bool)std::stoi(argv[2]);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<uint32_t> distribution(8, MAX_PAYLOAD);

    std::vector<Record> records;
    for (uint64_t i = 0; i < n; i++)
    {
        records.emplace_back(distribution(generator));

        if (print)
        {
            std::printf("record %lu: ", records.back().key());
            for (uint32_t i = 0; i < records.back().length(); i++)
                std::printf("%d ", records.back()[i]);
            std::printf("\n");
        }
    }
    serialize(records, "file.dat");
    std::printf("\n");

    records = deserialize("file.dat");
    if (print)
    {
        for (const auto& r : records)
        {
            std::printf("record %lu: ", r.key());
            for (uint32_t i = 0; i < r.length(); i++)
                std::printf("%u ", r[i]);
            std::printf("\n");
        }
    }

    return 0;
}
