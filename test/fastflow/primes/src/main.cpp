#include <cstdio>
#include <memory>
#include <string>

#include <ff/ff.hpp>

#include "components.hpp"
#include "timer.hpp"

using namespace ff;

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        std::printf("USAGE: %s <exp> <nworkers>\n", argv[0]);
        return 1;
    }
    uint64_t n = 1ULL << std::stoull(argv[1]);
    int nworkers = std::stoi(argv[2]);

    Source source(n);
    Sink sink;

    std::vector<std::unique_ptr<ff_node>> workers;
    for (int i = 0; i < nworkers; i++)
        workers.push_back(std::make_unique<Worker>());

    ff_Farm<uint64_t> farm(std::move(workers), source, sink);
    // farm.remove_collector();

    farm.set_scheduling_ondemand();

    // ff_Pipe<uint64_t> pipe(source, farm, sink);

    spm::timer timer;
    timer.start();
    farm.run_and_wait_end();

    std::printf("found %lu primes in %.4f seconds with %d workers\n",
                sink.counter(), timer.stop(), nworkers);

    return 0;
}
