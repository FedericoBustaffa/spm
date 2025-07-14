#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "mergesort.hpp"
#include "record.hpp"
#include "serialize.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <N> <L>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoull(argv[1]);     // # of records
    uint64_t limit = std::stoull(argv[2]); // max bytes

    // generate records
    std::vector<record> records = generate_records(n, 64);
    uint64_t bytes = mem_usage(records);
    std::printf("total bytes produced: %lu\n", bytes);

    // save unsorted records to a file
    std::ofstream out("records.bin", std::ios::binary | std::ios::trunc);
    dump(records, out);
    out.close();

    // partial reading with limits
    std::ifstream in("records.bin", std::ios::binary);
    std::vector<record> temp = load(in, limit);
    uint64_t i = 0;
    bytes = 0;
    while (!temp.empty())
    {
        // order the block
        mergesort(temp);
        assert(is_sorted(temp.begin(), temp.end()));

        // save the sorted block in a file
        std::stringstream ss;
        ss << "block_" << i++ << ".bin";
        std::ofstream block_file(ss.str());
        dump(temp, block_file);

        // check memory usage of a block
        uint64_t b = mem_usage(temp);
        bytes += b;
        assert(b <= limit);

        temp = load(in, limit);
    }
    in.close();
    std::printf("total bytes read: %lu\n", bytes);

    fs::remove("records.bin");

    return 0;
}
