#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <omp.h>
#include <vector>

#include "timer.hpp"

#define N 256

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    // image vector
    std::vector<uint8_t> image(std::atoi(argv[1]));
    std::generate(image.begin(), image.end(), []() { return rand() % N; });

    // final primary histogram
    std::vector<uint64_t> hist(N, 0);

    // local histograms for each thread
    int num_threads = omp_get_max_threads();
    std::vector<std::array<uint64_t, N>> local_hists(num_threads);

    spm::timer timer;
    timer.start();

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        std::array<uint64_t, N>& lh = local_hists[tid];
        for (size_t i = 0; i < lh.size(); i++)
            lh[i] = 0;

#pragma omp for
        for (size_t i = 0; i < image.size(); i++)
            lh[image[i]]++;
    }

    // sequential reduction
    for (size_t i = 0; i < N; i++)
        for (const auto& lh : local_hists)
            hist[i] += lh[i];

    std::printf("time %.2f\n", timer.stop());

    // for (size_t i = 0; i < hist.size(); i++)
    //     std::printf("%zu: %lu\n", i, hist[i]);

    return 0;
}
