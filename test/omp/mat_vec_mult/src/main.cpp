#include <cstdio>
#include <omp.h>

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    return 0;
}
