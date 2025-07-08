#include <algorithm>
#include <cassert>
#include <cstdio>

#include "mergesort.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <e>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoul(argv[1]);

    // generate shuffled vector
    std::vector<record> a = generate_records(n);
    std::vector<record> b = a;

    auto compare = [](const record& a, const record& b) {
        return a.key() < b.key();
    };

    spm::timer timer;

    // std::sort timing
    timer.start();
    std::sort(a.begin(), a.end(), compare);
    std::printf("std::sort time: %.4f\n", timer.stop());

    // mergesort
    timer.start();
    mergesort(b);
    std::printf("mergesort time: %.4f\n", timer.stop());

    // asserts to test correctness
    assert(std::equal(a.begin(), a.end(), b.begin(), b.end()));
    assert(std::is_sorted(a.begin(), a.end(), compare));
    assert(std::is_sorted(b.begin(), b.end(), compare));

    return 0;
}
