#include <cstdio>

#include "merge_sort.hpp"

int main(int argc, const char** argv)
{
    std::printf("sequential merge-sort %s\n", check() ? "ok" : "failed");

    return 0;
}
