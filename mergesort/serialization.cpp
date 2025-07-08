#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
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

    // generate shuffled v
    std::vector<record> a = generate_records(n);

    // check if sorted
    bool sorted = std::is_sorted(
        a.begin(), a.end(),
        [](const record& a, const record& b) { return a.key() < b.key(); });
    std::printf("starting array %s\n", sorted ? "sorted" : "unsorted");

    // serialize the array in a file
    std::ofstream out("records.dat", std::ios::binary);
    serialize(a, out);
    out.close();

    // deserialize
    std::ifstream in("records.dat", std::ios::binary);
    std::vector<record> b = deserialize(in);
    in.close();

    // compare the two to see if the serialization is correct
    bool equal = std::equal(a.begin(), a.end(), b.begin());
    std::printf("vectors are %s\n", equal ? "the same" : "different");

    return 0;
}
