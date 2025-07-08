#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "mergesort.hpp"
#include "record.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoul(argv[1]);

    // generate shuffled records
    std::vector<record> records = generate_records(n);

    // check if sorted
    bool sorted = std::is_sorted(
        records.begin(), records.end(),
        [](const record& a, const record& b) { return a.key() < b.key(); });

    std::printf("starting array %s\n", sorted ? "sorted" : "unsorted");

    return 0;
}
