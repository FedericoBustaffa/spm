#include <cstdint>
#include <vector>

#include <omp.h>

#include "timer.hpp"

std::vector<uint64_t> local(const std::vector<uint8_t>& image)
{
    std::vector<uint64_t> hist(256, 0);

    // local histograms for each thread
    int num_threads = omp_get_max_threads();
    std::vector<std::array<uint64_t, 256>> local_hists(num_threads);

    spm::timer timer;
    timer.start();

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        std::array<uint64_t, 256>& lh = local_hists[tid];
        for (size_t i = 0; i < lh.size(); i++)
            lh[i] = 0;

#pragma omp for
        for (size_t i = 0; i < image.size(); i++)
            lh[image[i]]++;
    }

    // sequential reduction
    for (size_t i = 0; i < 256; i++)
        for (const auto& lh : local_hists)
            hist[i] += lh[i];

    std::printf("time %.4f\n", timer.stop());

    return hist;
}
