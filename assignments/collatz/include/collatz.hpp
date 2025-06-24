#ifndef COLLATZ_HPP
#define COLLATZ_HPP

#include <cstddef>
#include <cstdint>

struct range
{
    uint64_t a, b;

    range(uint64_t a, uint64_t b) : a(a), b(b) {}

    inline uint64_t length() const { return (b - a) + 1; }
};

uint64_t collatz_steps(uint64_t n);

uint64_t sequential(const range& range);

uint64_t block_cyclic(size_t workers_num, size_t chunksize, const range& range);

uint64_t dynamic(size_t workers_num, const range& range);

#endif
