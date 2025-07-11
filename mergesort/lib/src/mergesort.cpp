#include "mergesort.hpp"

#include <cmath>
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
    std::ifstream file1(filepath1, std::ios::binary);
    std::ifstream file2(filepath2, std::ios::binary);

    while (!file1.eof() && !file2.eof())
    {
        std::vector<record> blk1 = load_vector(file1, limit / 4);
        std::vector<record> blk2 = load_vector(file2, limit / 4);

        std::vector<record> result(blk1.size() + blk2.size());

        dump_vector(result, "merged.dat");
    }
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
        ss << "block" << block_counter++ << ".bin";
        dump_vector(block, ss.str().c_str());
        ss.str("");
        ss.clear();
    }

    for (size_t i = 0; i < block_counter; i += 2)
    {
        ss << "block" << i << ".bin";
        const char* filepath1 = std::move(ss.str().c_str());
        const char* filepath2 = std::move(ss.str().c_str());
        merge_blocks(filepath1, filepath2, limit);
    }
}
