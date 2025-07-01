#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

std::vector<uint64_t> sequential(const std::vector<uint8_t>& image);
std::vector<uint64_t> local(const std::vector<uint8_t>& image);

int main(int argc, const char** argv)
{
    if (argc != 2)
    {
        std::printf("USAGE: %s <exp>\n", argv[0]);
        return 1;
    }

    uint64_t n = 1ULL << std::stoull(argv[1]);

    // image vector
    std::vector<uint8_t> image(n);
    std::generate(image.begin(), image.end(), []() { return rand() % 256; });
    std::printf("%lu numbers array generated\n", n);

    std::vector<uint64_t> sh = sequential(image);
    std::vector<uint64_t> lh = local(image);

    for (size_t i = 0; i < sh.size(); i++)
    {
        if (sh[i] != lh[i])
            std::printf("sequential[%zu]: %lu != local[%zu]: %lu\n", i, sh[i],
                        i, lh[i]);
    }

    return 0;
}
