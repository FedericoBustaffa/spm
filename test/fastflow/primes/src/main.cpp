#include <cstdio>
#include <memory>
#include <string>

#include <ff/ff.hpp>

#include "timer.hpp"

using namespace ff;

class Source : public ff_node_t<uint64_t>
{
public:
    Source(uint64_t n) : n(n) {}

    uint64_t* svc(uint64_t*) override
    {
        for (uint64_t i = 2; i < n; i++)
            ff_send_out(new uint64_t(i));

        return EOS;
    }

private:
    uint64_t n;
};

class Worker : public ff_node_t<uint64_t>
{
public:
    uint64_t* svc(uint64_t* n) override
    {
        if (*n < 2)
        {
            delete n;
            return GO_ON;
        }

        for (uint64_t i = 2; i * i <= *n; i++)
        {
            if (*n % i == 0)
            {
                delete n;
                return GO_ON;
            }
        }

        delete n;
        ff_send_out(new uint64_t(1));

        return GO_ON;
    }
};

class Sink : public ff_minode_t<uint64_t>
{
public:
    uint64_t* svc(uint64_t* n) override
    {
        m_counter += *n;
        delete n;

        return GO_ON;
    }

    inline uint64_t counter() const { return m_counter; }

private:
    uint64_t m_counter = 0;
};

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

    ff_Farm<uint64_t> farm(std::move(workers));
    ff_Pipe<uint64_t> pipe(source, farm, sink);

    farm.set_scheduling_ondemand();

    pipe.run_and_wait_end();
    // std::printf("found %lu primes in %.4f seconds with %d workers\n",
    //             sink.counter(), pipe.ffTime(), nworkers);

    std::printf("found %lu primes in %.4f seconds with %d workers\n",
                sink.counter(), farm.ffTime(), nworkers);

    return 0;
}
