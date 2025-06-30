#include <cstdio>
#include <string>

#include "matrix.hpp"
#include "utility.hpp"
#include "vector.hpp"

vector product(const matrix& A, const vector& x) {}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <n>\n", argv[0]);
        return 1;
    }

    size_t n = std::stoull(argv[1]);

    matrix A(generate_matrix(n, n, 0.8f), n, n);
    vector x(generate_vector(n, 0.8f), n);

    vector y = product(A, x);

    return 0;
}
