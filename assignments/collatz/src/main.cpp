#include <regex>
#include <vector>

#include "collatz.hpp"

std::vector<std::pair<uint64_t, uint64_t>> parse_ranges(int argc,
                                                        const char** argv)
{
    std::vector<std::pair<uint64_t, uint64_t>> ranges;
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

    std::vector<std::pair<uint64_t, uint64_t>> ranges =
        parse_ranges(argc, argv);

    double stime = sequential(ranges);
    std::printf("sequential time: %.4f ns\n\n", stime);

    double static_time = static_schedule(p, ranges);
    std::printf("static time: %.4f ns\n\n", static_time);

    double dynamic_time = dynamic_schedule(p, ranges);
    std::printf("dynamic time: %.4f ns\n\n", dynamic_time);

    std::printf("- - - - - - - - -\n");
    std::printf("static schedule speedup: %.2f\n", (stime / static_time));
    std::printf("dynamic schedule speedup: %.2f\n", (stime / dynamic_time));

    return 0;
}
