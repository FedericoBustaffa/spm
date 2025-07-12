#include <algorithm>
#include <cassert>
#include <cstdio>

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

    // generate 2 blocks
    std::vector<record> blk1 = generate_records(n);
    std::vector<record> blk2 = generate_records(n);

    // sort blocks
    mergesort(blk1);
    mergesort(blk2);

    // save sorted records to a file
    dump_vector(blk1, "blk1.bin");
    dump_vector(blk2, "blk2.bin");

    // merge the 2 blocks and create the array for comparison
    std::vector<record> correct(blk1.size() + blk2.size());
    std::merge(blk1.begin(), blk1.end(), blk2.begin(), blk2.end(),
               correct.begin());

    merge_blocks("blk1.bin", "blk2.bin", limit);

    return 0;
}
