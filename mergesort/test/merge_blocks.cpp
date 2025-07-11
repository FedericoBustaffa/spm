#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>

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
    if (argc != 2)
    {
        std::printf("USAGE: %s <N>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoull(argv[1]); // # of records

    auto compare = [](const record& a, const record& b) {
        return a.key() < b.key();
    };

    // generate records
    std::vector<record> records = generate_records(n);

    std::vector<record> blk1, blk2;
    for (size_t i = 0; i < records.size() / 2; i++)
        blk1.push_back(records[i]);

    for (size_t i = records.size() / 2; i < records.size(); i++)
        blk2.push_back(records[i]);

    std::printf("blk1 size: %lu\n", blk1.size());
    std::printf("blk2 size: %lu\n", blk2.size());

    mergesort(blk1);
    mergesort(blk2);
    assert(std::is_sorted(blk1.begin(), blk1.end(), compare));
    assert(std::is_sorted(blk2.begin(), blk2.end(), compare));
    serialize(blk1, "blk1.dat");
    serialize(blk2, "blk2.dat");

    // test
    // merge_blocks("blk1.dat", "blk2.dat");

    // final clean up
    std::filesystem::remove("blk1.dat");
    std::filesystem::remove("blk2.dat");

    return 0;
}
