#include <algorithm>
#include <cassert>
#include <cstdio>
#include <filesystem>

#include "mergesort.hpp"
#include "serialize.hpp"
#include "timer.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

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
    std::vector<record> a = generate_records(n, 256);
    dump(a, "vector.bin");

    // sort and generate a file with sorted array
    spm::timer timer;
    timer.start();
    mergesort("vector.bin", limit);
    double time = timer.stop();

    // check if the array is sorted correctly
    std::vector<record> result = load("vector.bin");

    assert(result.size() == a.size());
    assert(!std::equal(a.begin(), a.end(), result.begin(), result.end()));
    assert(!std::is_sorted(a.begin(), a.end()));
    assert(std::is_sorted(result.begin(), result.end()));

    std::printf(
        "file size: %lu, n: %lu, limit: %lu bytes, time: %.4f seconds\n",
        fs::file_size("vector.bin"), n, limit, time);

    return 0;
}
