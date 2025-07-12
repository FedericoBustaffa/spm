#include <algorithm>
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
    dump_vector(a, "vector.bin");
    for (const auto& i : a)
        std::printf("%lu\n", i.key());

    // sort and generate a file with sorted array
    mergesort("vector.bin", limit);

    // check if the array is sorted correctly
    std::vector<record> result =
        load_vector("block_0.bin", a.size() * (12 + MAX_PAYLOAD));

    for (const auto& r : result)
        std::printf("%lu\n", r.key());

    assert(result.size() == a.size());
    assert(!std::is_sorted(a.begin(), a.end()));
    assert(std::is_sorted(result.begin(), result.end()));

    return 0;
}
