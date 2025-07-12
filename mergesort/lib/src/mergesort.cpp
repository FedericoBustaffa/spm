#include "mergesort.hpp"

#include <cmath>
#include <filesystem>
#include <sstream>

#include "serialize.hpp"

void merge(std::vector<record>& v, size_t first, size_t middle, size_t last,
           std::vector<record>& support)
{
    size_t left = first;
    size_t right = middle;
    size_t i = first;

    // merge
    while (left < middle && right < last)
    {
        if (v[left].key() <= v[right].key())
            support[i++] = std::move(v[left++]);
        else
            support[i++] = std::move(v[right++]);
    }

    // consume the the remaining array
    if (left >= middle)
        while (right < last)
            support[i++] = std::move(v[right++]);
    else
        while (left < middle)
            support[i++] = std::move(v[left++]);

    // copy the support array in the original
    for (size_t i = first; i < last; i++)
        v[i] = std::move(support[i]);
}

void sort(std::vector<record>& v, size_t first, size_t last,
          std::vector<record>& support)
{
    if (last - first <= 1)
        return;

    size_t middle = std::ceil((first + last) / 2);
    sort(v, first, middle, support);
    sort(v, middle, last, support);

    merge(v, first, middle, last, support);
}

void mergesort(std::vector<record>& v)
{
    std::vector<record> support(v.size());
    sort(v, 0, v.size(), support);
}

void merge_blocks(const char* filepath1, const char* filepath2, uint64_t limit)
{
    std::ifstream in1(filepath1, std::ios::binary);
    std::ifstream in2(filepath2, std::ios::binary);
    std::ofstream out("merged.bin", std::ios::binary | std::ios::app);

    std::vector<record> blk1 = load_vector(in1, limit / 4);
    std::vector<record> blk2 = load_vector(in2, limit / 4);
    std::vector<record> result(limit / 2);

    size_t i1 = 0, i2 = 0, r = 0;

    while (!blk1.empty() && !blk2.empty())
    {
        while (i1 < blk1.size() && i2 < blk2.size() && r < result.size())
        {
            if (blk1[i1].key() <= blk2[i2].key())
                result[r++] = std::move(blk1[i1++]);
            else
                result[r++] = std::move(blk2[i2++]);
        }

        if (i1 >= blk1.size())
        {
            blk1 = load_vector(in1, limit / 4);
            i1 = 0;
        }
        else
        {
            blk2 = load_vector(in2, limit / 4);
            i2 = 0;
        }

        if (r >= result.size())
        {
            dump_vector(result, out);
            result = std::vector<record>(limit / 2);
            r = 0;
        }
    }

    // TODO: handle when one of the two files is totally consumed

    std::filesystem::remove(filepath1);
    std::filesystem::remove(filepath2);
    std::filesystem::rename("merged.bin", filepath1);
}

void mergesort(const char* filepath, uint64_t limit)
{
    std::ifstream file(filepath, std::ios::binary);
    std::stringstream ss;

    std::vector<record> block;
    size_t block_counter = 0;
    while (!file.eof())
    {
        // read a block
        block = load_vector(file, limit / 2);

        // sort
        mergesort(block);

        // save the sorted block to a file
        ss << "block_" << block_counter++ << ".bin";
        dump_vector(block, ss.str().c_str());
        ss.str("");
        ss.clear();
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
    // std::filesystem::rename("block_0.bin", filepath);
}
