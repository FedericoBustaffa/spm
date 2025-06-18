#include <regex>
#include <vector>

#include "collatz.hpp"

std::vector<std::pair<uint64_t, uint64_t>> parse_ranges(int argc,
                                                        const char** argv)
{
    std::regex regex(R"(^(\d+)-(\d+)$)");
    std::smatch match;

    for (int i = 1; i < argc; i++)
    {
        std::string arg(argv[i]);
        if (std::regex_match(arg, match, regex))
        {
            uint64_t start = std::stoull(match[1]);
            uint64_t end = std::stoull(match[2]);
            if (start >= end)
                std::printf("%s: range start >= than range end\n", arg.c_str());
            else
                std::printf("%s: good format\n", arg.c_str());
        }
        else
            std::printf("%s: bad format\n", arg.c_str());
    }
    return {};
}

int main(int argc, const char** argv)
{
    if (argc != 4)
    {
        std::printf("USAGE: ./run.out <range1> <range2> <range3>\n");
        return 1;
    }

    std::vector<std::pair<uint64_t, uint64_t>> ranges =
        parse_ranges(argc, argv);

    double stime = sequential(ranges);
    std::printf("sequential time: %.4f ns\n", stime);

    double static_time = static_schedule(ranges);
    std::printf("sequential time: %.4f ns\n", static_time);

    double dynamic_time = dynamic_schedule(ranges);
    std::printf("sequential time: %.4f ns\n", dynamic_time);

    std::printf("static schedule speedup: %.2f\n", (stime / static_time));
    std::printf("dynamic schedule speedup: %.2f\n", (stime / dynamic_time));

    return 0;
}
