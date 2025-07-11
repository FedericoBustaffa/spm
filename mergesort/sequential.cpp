#include <cassert>
#include <cstdio>

#include "mergesort.hpp"
#include "serialize.hpp"
#include "utils.hpp"

int main(int argc, const char** argv)
{
    if (argc < 3)
    {
        std::printf("USAGE: %s <E> <L>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoul(argv[1]);
    uint64_t limit = std::stoul(argv[2]);

    // generate and save shuffled vector
    std::vector<record> a = generate_records(n);
    serialize(a, "vector.bin");

    mergesort("vector.bin", limit);

    return 0;
}
