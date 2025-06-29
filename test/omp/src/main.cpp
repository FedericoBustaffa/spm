#include <cstdint>
#include <cstdio>
#include <omp.h>
#include <vector>

#include "timer.hpp"

using namespace std::chrono_literals;

std::vector<uint64_t> vec_add(const std::vector<uint64_t>& a,
                              const std::vector<uint64_t>& b)
{
    std::vector<uint64_t> res(a.size());

#pragma omp parallel for
    for (size_t i = 0; i < a.size(); i++)
        res[i] = a[i] + b[i];

    return res;
}

uint64_t sum(const std::vector<uint64_t>& v)
{
    uint64_t s = 0;

#pragma omp parallel for reduction(+ : s)
    for (size_t i = 0; i < v.size(); i++)
        s += v[i];

    return s;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    size_t n = 1ULL << std::stoul(argv[1]);

    std::vector<uint64_t> a;
    std::vector<uint64_t> b;
    for (size_t i = 0; i < n; i++)
    {
        a.push_back(i);
        b.push_back(i * 2);
    }

    spm::timer timer;
    timer.start();

    std::vector<uint64_t> c = vec_add(a, b);
    uint64_t s = sum(c);

    std::printf("sum: %lu in %.4f seconds\n", s, timer.stop());

    return 0;
}
