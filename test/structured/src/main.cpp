#include <cstdio>

#include "pipeline.hpp"

double f(double x) { return x * 2; }

double g(double x) { return x + 3; }

int main(int argc, const char** argv)
{
    spm::pipeline pl;

    pl.add_stage(f);
    pl.add_stage(g);

    double values[5] = {0, 1, 2, 3, 4};

    double* result = pl.run(values);

    return 0;
}
