#include <cstdio>

#include "node.hpp"
#include "timer.hpp"

double f(double x) { return x * 2.0; }

double g(double x) { return x + 3.5; }

double h(double x) { return x - 5.0; }

int main(int argc, const char** argv)
{
    spm::timer timer;
    timer.start();
    for (int i = 0; i < 100000; i++)
        f(g(h(10.0)));
    std::printf("seq time: %.6f\n", timer.stop());

    // independent nodes
    spm::sink_node<double, double> sink(f);
    spm::node<double, double> mid(g);
    spm::source_node<double, double> source(h);

    // connect two nodes with a single queue
    source.connect_to(mid);
    mid.connect_to(sink);

    // start a computation
    timer.start();
    for (int i = 0; i < 1000; i++)
        source.send(10.0);

    // fetch the result
    for (int i = 0; i < 1000; i++)
        sink.recv();
    std::printf("seq time: %.6f\n", timer.stop());

    return 0;
}
