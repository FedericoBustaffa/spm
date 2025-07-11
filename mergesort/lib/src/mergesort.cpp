#include "mergesort.hpp"

#include <cmath>

void merge(record* v, size_t first, size_t middle, size_t last, record* support)
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

void sort(record* v, size_t first, size_t last, record* support)
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
    record* support = new record[v.size()];
    sort(v.data(), 0, v.size(), support);
    delete[] support;
}
