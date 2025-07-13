#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "record.hpp"
#include "serialize.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <N>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoul(argv[1]);

    // generate shuffled vector
    std::vector<record> a = generate_records(n, 64);

    // check if sorted
    assert(!std::is_sorted(
        a.begin(), a.end(),
        [](const record& a, const record& b) { return a.key() < b.key(); }));

    // serialize the array in a file
    std::ofstream out("records.dat", std::ios::binary);
    dump_vector(a, out);
    out.close();

    // deserialize
    std::ifstream in("records.dat", std::ios::binary);
    std::vector<record> b = load_vector(in, 10000000);
    in.close();

    // compare the two to see if the serialization is correct
    assert(std::equal(a.begin(), a.end(), b.begin()));

    // delete the created file
    fs::remove("records.dat");

    return 0;
}
