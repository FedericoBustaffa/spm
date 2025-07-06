#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "record.hpp"
#include "utils.hpp"

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }
    uint64_t n = std::stoul(argv[1]);

    // generate and serialize data
    std::vector<record> records = generate_records(n);
    serialize(records);

    // deserialize data
    std::printf("*********************\n");
    std::stringstream ss;
    ss << "records_" << n << ".dat";
    std::vector<record> readed = deserialize(ss.str().c_str());
    assert(readed.size() == records.size());

    for (size_t i = 0; i < readed.size(); i++)
    {
        if (records[i] != readed[i])
        {
            std::printf("readed diff from written\n");
            return 1;
        }
    }
    std::printf("records serialized correctly\n");

    // sort
    std::sort(
        records.begin(), records.end(),
        [](const record& a, const record& b) { return a.key() < b.key(); });

    for (size_t i = 0; i < records.size(); i++)
        std::printf("%lu\n", records[i].key());

    return 0;
}
