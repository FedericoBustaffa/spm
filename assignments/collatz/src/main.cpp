#include <cmath>
#include <regex>

#include "collatz.hpp"

std::vector<range> parse_ranges(int argc, const char** argv)
{
    std::vector<range> ranges;
    std::regex regex(R"(^(\d+)-(\d+)$)");
    std::smatch match;

    for (int i = 2; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (std::regex_match(arg, match, regex))
        {
            uint64_t start = std::stoull(match[1]);
            uint64_t end = std::stoull(match[2]);
            if (start <= 0 || end <= 0)
                std::printf("%s: ranges have to be > 0\n", arg.c_str());
            else if (start >= end)
                std::printf("%s: range start >= than range end\n", arg.c_str());
            else
                ranges.emplace_back(start, end);
        }
        else
            std::printf("%s: bad format\n", arg.c_str());
    }

    return ranges;
}

int main(int argc, const char** argv)
{
    if (argc <= 2)
    {
        std::printf("USAGE: %s <workers> <range1> [ranges...]\n", argv[0]);
        return 1;
    }

    std::regex regex(R"(^\d+$)");
    std::smatch match;
    std::string first_arg(argv[1]);
    size_t p;

    if (std::regex_match(first_arg, match, regex))
        p = std::atoi(argv[1]);
    else
    {
        std::printf("USAGE: %s <workers> <range1> [ranges...]\n", argv[0]);
        return 1;
    }

    std::vector<range> ranges = parse_ranges(argc, argv);

    // sequential
    double stime = 0.0;
    for (const auto& r : ranges)
        stime += sequential(r);
    std::printf("sequential time: %.4f s\n\n", stime);

    // block
    double btime = 0.0;
    for (const auto& r : ranges)
        btime += block_cyclic(p, std::ceil(r.length() / p), r);
    std::printf("block time: %.4f s\n", btime);
    std::printf("block speedup: %.2f\n\n", (stime / btime));

    // cyclic time
    double ctime = 0.0;
    for (const auto& r : ranges)
        ctime += block_cyclic(p, 1, r);
    std::printf("cyclic time: %.4f s\n", ctime);
    std::printf("cyclic speedup: %.2f\n\n", (stime / ctime));

    // block-cyclic time
    double bctime = 0.0;
    for (const auto& r : ranges)
        bctime += block_cyclic(p, std::ceil(std::ceil(r.length() / p) / 4), r);
    std::printf("block-cyclic time: %.4f s\n", bctime);
    std::printf("block-cyclic speedup: %.2f\n\n", (stime / bctime));

    // dynamic
    double dtime = 0.0;
    for (const auto& r : ranges)
        dtime += dynamic(p, r);
    std::printf("dynamic time: %.4f s\n", dtime);
    std::printf("dynamic speedup: %.2f\n", (stime / dtime));

    return 0;
}
