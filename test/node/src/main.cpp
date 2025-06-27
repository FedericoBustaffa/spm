#include <cstdio>

#include "node.hpp"

double f(double x) { return x * 2; }

double g(double x) { return x + 3; }

int main(int argc, const char** argv)
{
    std::printf("correct result %.2f\n", f(g(10.0)));

    spm::node<double, double> n1(f);
    spm::node<double, double> n2(g);

    // from now on n2 output will be redirected to n1
    n2.connect_to<double>(n1);

    std::printf("node connection: %.2f\n", n1(n2(10)));

    return 0;
}
