#include "mergesort.hpp"

#include <filesystem>
#include <sstream>

#include "serialize.hpp"

namespace fs = std::filesystem;

void merge_blocks(const char* filepath1, const char* filepath2, uint64_t limit)
{
    std::ifstream in1(filepath1, std::ios::binary);
    std::ifstream in2(filepath2, std::ios::binary);
    std::ofstream out("merged.bin", std::ios::binary | std::ios::app);

    std::vector<record> blk1 = load(in1, limit / 4);
    std::vector<record> blk2 = load(in2, limit / 4);

    std::vector<record> buffer;
    buffer.reserve((limit / 2) / 20);
    uint64_t bufsize = 0; // in bytes

    size_t i1 = 0, i2 = 0;
    while (!blk1.empty() && !blk2.empty())
    {
        while (i1 < blk1.size() && i2 < blk2.size() && bufsize < limit / 2)
        {
            if (blk1[i1].key() <= blk2[i2].key())
                buffer.push_back(std::move(blk1[i1++]));
            else
                buffer.push_back(std::move(blk2[i2++]));

            bufsize +=
                sizeof(uint64_t) + sizeof(uint32_t) + buffer.back().length();
        }

        if (i1 >= blk1.size())
        {
            blk1 = load(in1, limit / 4);
            i1 = 0;
        }
        else if (i2 >= blk2.size())
        {
            blk2 = load(in2, limit / 4);
            i2 = 0;
        }

        if (bufsize >= limit / 2)
        {
            dump(buffer, out);
            buffer.clear();
            bufsize = 0;
        }
    }

    // create some convenience aliases to simplify the code
    std::vector<record>& last_blk = blk1.empty() ? blk2 : blk1;
    size_t idx = blk1.empty() ? i2 : i1;
    std::ifstream& in_last = blk1.empty() ? in2 : in1;
    bufsize = 0;

    // consume the remaining file
    while (!last_blk.empty())
    {
        while (idx < last_blk.size() && bufsize < limit / 2)
        {
            buffer.push_back(std::move(last_blk[idx++]));
            bufsize +=
                sizeof(uint64_t) + sizeof(uint32_t) + buffer.back().length();
        }

        if (idx >= last_blk.size())
        {
            last_blk = load(in_last, limit / 2);
            idx = 0;
        }
        else
        {
            dump(buffer, out);
            buffer.clear();
            bufsize = 0;
        }
    }

    dump(buffer, out);
    in1.close();
    in2.close();
    out.close();

    fs::remove(filepath1);
    fs::remove(filepath2);
    fs::rename("merged.bin", filepath1);
}

void mergesort(const char* filepath, uint64_t limit)
{
    std::ifstream file(filepath, std::ios::binary);
    std::stringstream ss;

    std::vector<record> block = load(file, limit / 2);
    size_t block_counter = 0;
    while (!block.empty())
    {
        // sort
        mergesort(block);

        // save the sorted block to a file
        ss << "block_" << block_counter++ << ".bin";
        dump(block, ss.str().c_str());
        ss.str("");
        ss.clear();

        // read the next block
        block = load(file, limit / 2);
    }

    std::string filepath1, filepath2;
    for (size_t i = 0; i < std::ceil(std::log2(block_counter)); i++)
    {
        std::printf("- - - - - level %lu - - - - -\n", i);
        for (size_t j = 0; j < block_counter; j += 2 * std::pow(2, i))
        {
            // compute second file index
            size_t j2 = j + std::pow(2, i);
            if (j2 < block_counter)
            {
                ss << "block_" << j << ".bin";
                filepath1 = ss.str();
                ss.str("");
                ss.clear();

                ss << "block_" << j2 << ".bin";
                filepath2 = ss.str();
                ss.str("");
                ss.clear();

                std::printf("merge %s with %s\n", filepath1.c_str(),
                            filepath2.c_str());

                merge_blocks(filepath1.c_str(), filepath2.c_str(), limit);
            }
        }
    }
    fs::rename("block_0.bin", filepath);
}
