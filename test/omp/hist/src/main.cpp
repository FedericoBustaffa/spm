#include <chrono>
#include <cstdint>
#include <cstdio>
#include <omp.h>
#include <vector>

std::vector<uint64_t> sequential(const std::vector<uint8_t>& image);
std::vector<uint64_t> local(const std::vector<uint8_t>& image);
// std::vector<uint64_t> task(const std::vector<uint8_t>& image);

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    // image vector
    std::vector<uint8_t> image(std::atoi(argv[1]));
    std::generate(image.begin(), image.end(), []() { return rand() % 256; });

    std::vector<uint64_t> sh = sequential(image);
    std::vector<uint64_t> lh = local(image);
    // std::vector<uint64_t> th = task(image);

    for (size_t i = 0; i < sh.size(); i++)
    {
        if (sh[i] != lh[i])
            std::printf("sequential[%zu]: %lu != local[%zu]: %lu\n", i, sh[i],
                        i, lh[i]);

        // if (sh[i] != th[i])
        //     std::printf("sequential[%zu]: %lu != task[%zu]: %lu\n", i, sh[i],
        //     i,
        //                 th[i]);
    }

    return 0;
}
