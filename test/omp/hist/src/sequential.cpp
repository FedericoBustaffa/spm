#include <cstdint>
#include <vector>

#include "timer.hpp"

std::vector<uint64_t> sequential(const std::vector<uint8_t>& image)
{
    std::vector<uint64_t> hist(256, 0);

    spm::timer timer;
    timer.start();

    for (size_t i = 0; i < image.size(); i++)
        hist[image[i]]++;

    std::printf("time %.4f\n", timer.stop());

    return hist;
}
