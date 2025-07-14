#include <algorithm>
#include <cassert>
#include <cstdio>

#include "mergesort.hpp"
#include "record.hpp"
#include "serialize.hpp"
#include "utils.hpp"

#define MAX_PAYLOAD 64

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <E> <L>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoull(argv[1]); // # of records
    uint64_t limit = std::stoull(argv[2]);     // max bytes

    // generate 2 blocks
    std::vector<record> blk1 = generate_records(n, MAX_PAYLOAD);
    std::vector<record> blk2 = generate_records(n, MAX_PAYLOAD);

    // sort blocks
    mergesort(blk1);
    mergesort(blk2);

    // save sorted records to a file
    dump(blk1, "blk1.bin");
    dump(blk2, "blk2.bin");

    // merge the 2 blocks and create the array for comparison
    std::vector<record> correct(blk1.size() + blk2.size());
    std::merge(blk1.begin(), blk1.end(), blk2.begin(), blk2.end(),
               correct.begin());

    // merge 2 blocks with implemented function
    merge_blocks("blk1.bin", "blk2.bin", limit);
    std::vector<record> result = load("blk1.bin");

    // checks
    assert(std::is_sorted(correct.begin(), correct.end()));
    assert(std::is_sorted(result.begin(), result.end()));
    assert(std::equal(correct.begin(), correct.end(), result.begin(),
                      result.end()));

    return 0;
}
