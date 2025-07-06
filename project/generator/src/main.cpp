#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "Record.hpp"
#include "serialize.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    uint64_t n = std::stoul(argv[1]);
    std::printf("generating %lu records with MAX_PAYLOAD=%d\n", n, MAX_PAYLOAD);

    // create records
    std::vector<Record> records;
    for (uint64_t i = 0; i < n; i++)
        records.emplace_back((8 + i) % MAX_PAYLOAD);

    // shuffle records
    std::mt19937 generator(42);
    std::shuffle(records.begin(), records.end(), generator);
    serialize(records, "file.dat");

    std::vector<Record> readed = deserialize("file.dat");
    assert(readed.size() == records.size());

    for (size_t i = 0; i < readed.size(); i++)
    {
        if (records[i] != readed[i])
        {
            std::printf("readed diff from written\n");
            return 1;
        }
    }
    std::printf("records serialized correctly\n");

    return 0;
}
