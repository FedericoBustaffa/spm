#include <cstdio>

#include "node.hpp"

double f(double x) { return x * 2; }

double g(double x) { return x + 3; }

int main(int argc, const char** argv)
{
    spm::node<double, double> n1(f);
    spm::node<double, double> n2(g);

    std::printf("%.2f\n", n1(n2(n1(10))));

    return 0;
}
