#include "serialize.hpp"

#include <cstring>

#include "utils.hpp"

void dump(const std::vector<record>& records, std::ofstream& file)
{
    uint64_t bufsize = mem_usage(records);
    char* buffer = new char[bufsize];
    size_t index = 0;

    for (const auto& r : records)
    {
        std::memcpy(buffer + index, &r.key(), sizeof(uint64_t));
        index += sizeof(uint64_t);

        std::memcpy(buffer + index, &r.length(), sizeof(uint32_t));
        index += sizeof(uint32_t);

        std::memcpy(buffer + index, r.payload(), r.length());
        index += r.length();
    }

    file.write(buffer, bufsize);
    delete[] buffer;
}

void dump(const std::vector<record>& records, const char* filepath)
{
    std::ofstream out(filepath, std::ios::binary);
    dump(records, out);
}
