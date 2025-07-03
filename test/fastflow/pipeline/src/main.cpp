#include <cstdio>
#include <string>
#include <thread>

#include <ff/ff.hpp>

#include "components.hpp"
#include "timer.hpp"

using namespace std::chrono_literals;
using namespace ff;

uint64_t f(uint64_t x)
{
    std::this_thread::sleep_for(50ms);
    return x * 2.0;
}

uint64_t g(uint64_t x)
{
    std::this_thread::sleep_for(50ms);
    return x + 5.5;
}

uint64_t h(uint64_t x)
{
    std::this_thread::sleep_for(50ms);
    return x - 1.0;
}

uint64_t sequential(uint64_t n)
{
    uint64_t s = 0;
    for (uint64_t i = 0; i < n; i++)
        s += f(g(h(f(g(h(i))))));

    return s;
}

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <exp> <par>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoull(argv[1]);

    bool par = (bool)std::stoi(argv[2]);

    spm::timer timer;
    if (!par)
    {
        timer.start();
        uint64_t result = sequential(n);
        double time = timer.stop();
        std::printf("sequential result: %lu in %.4f seconds\n", result, time);
    }
    else
    {
        Source source(n);
        Sink sink;
        Worker s1(f), s2(h), s3(g), s4(f), s5(g), s6(h);
        ff_Pipe<uint64_t> pipe(source, s1, s2, s3, s4, s5, s6, sink);

        timer.start();
        pipe.run_and_wait_end();
        double time = timer.stop();

        uint64_t result = sink.result();
        std::printf("parallel result %lu in %.4f seconds\n", result, time);
    }

    return 0;
}
