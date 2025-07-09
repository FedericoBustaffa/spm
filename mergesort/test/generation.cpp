#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "record.hpp"
#include "utils.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <N>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoul(argv[1]);

    // generate shuffled records
    std::vector<record> records = generate_records(n);
    for (const auto& r : records)
    {
        assert(r.length() == std::strlen(r.payload()));
        std::printf("key: %lu - length: %u\n", r.key(), r.length());
    }

    // check if sorted
    assert(!std::is_sorted(
        records.begin(), records.end(),
        [](const record& a, const record& b) { return a.key() < b.key(); }));

    return 0;
}
