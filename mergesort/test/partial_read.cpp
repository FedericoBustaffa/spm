#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "record.hpp"
#include "utils.hpp"

uint64_t mem_usage(const std::vector<record>& v)
{
    uint64_t bytes = 0;
    for (const auto& i : v)
        bytes += sizeof(uint64_t) + sizeof(uint32_t) + i.length();

    return bytes;
}

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <N> <L>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoull(argv[1]);     // # of records
    uint64_t limit = std::stoull(argv[2]); // max bytes

    // limit must be at least key + length + MAX_PAYLOAD
    uint64_t min_limit = sizeof(uint64_t) + sizeof(uint32_t) + MAX_PAYLOAD;
    limit = limit < min_limit ? min_limit : limit;

    // generate and save records to a file
    std::vector<record> records = generate_records(n);
    uint64_t bytes = mem_usage(records);
    std::printf("total bytes produced: %lu\n", bytes);

    std::ofstream out("records.dat", std::ios::binary);
    dump(records, out);
    out.close();

    // partial reading with limits
    std::vector<record> temp;
    std::vector<record> records2;
    std::ifstream in("records.dat", std::ios::binary);
    while (!in.eof())
    {
        temp = load(in, limit);
        if (records.size() == 0)
            break;

        bytes = mem_usage(temp);
        for (const auto& i : temp)
            records2.push_back(std::move(i));

        assert(bytes <= limit);
        std::printf("bytes: %lu\n", bytes);
    }
    in.close();

    assert(std::equal(records.begin(), records.end(), records2.begin(),
                      records2.end()));

    std::filesystem::remove("records.dat");

    return 0;
}
