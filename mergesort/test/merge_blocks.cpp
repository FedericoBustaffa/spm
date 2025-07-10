#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "mergesort.hpp"
#include "record.hpp"
#include "serialize.hpp"
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

    auto compare = [](const record& a, const record& b) {
        return a.key() < b.key();
    };

    // generate records
    std::vector<record> records = generate_records(n);

    // save unsorted records to a file
    std::ofstream out("records.dat", std::ios::binary);
    serialize(records, out);
    out.close();

    // sort records
    mergesort(records);
    assert(std::is_sorted(records.begin(), records.end(), compare));
    uint64_t bytes = mem_usage(records);
    std::printf("total bytes produced: %lu\n", bytes);

    // partial reading with limits
    std::vector<record> temp;
    std::ifstream in("records.dat", std::ios::binary);
    uint64_t i = 0;
    bytes = 0;
    while (!in.eof())
    {
        temp = deserialize(in, limit);
        if (records.size() == 0)
            break;

        // order the block
        mergesort(temp);
        assert(is_sorted(temp.begin(), temp.end(), compare));

        // save the sorted block in a file
        std::stringstream ss;
        ss << "block_" << i++ << ".dat";
        std::ofstream block_file(ss.str());
        serialize(temp, block_file);
        block_file.close();

        // check memory usage of a block
        uint64_t b = mem_usage(temp);
        bytes += b;
        assert(b <= limit);
    }
    in.close();
    std::printf("total bytes read: %lu\n", bytes);

    std::filesystem::path generated("records.dat");
    std::filesystem::remove(generated);

    return 0;
}
