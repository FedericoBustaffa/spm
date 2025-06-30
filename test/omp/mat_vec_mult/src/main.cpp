#include <cstdio>
#include <omp.h>
#include <string>

#include "matrix.hpp"
#include "timer.hpp"
#include "utility.hpp"
#include "vector.hpp"

vector product(const matrix& A, const vector& x)
{
    int n_threads = omp_get_max_threads();
    std::vector<vector> local_vectors;
    local_vectors.reserve(n_threads);
    for (int i = 0; i < n_threads; i++)
        local_vectors.emplace_back(x.length());

    spm::timer timer;
    timer.start();
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        vector& local = local_vectors[tid];
#pragma omp for
        for (size_t i = 0; i < A.size(); i++)
            local[A.row(i)] += A[i] * x[A.col(i)];
    }

    vector res(x.length());
    for (size_t i = 0; i < res.length(); i++)
        for (const auto& lv : local_vectors)
            res[i] += lv[i];

    double time = timer.stop();
    std::printf("%d threads time: %.4f\n", n_threads, time);

    return res;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        std::printf("USAGE: %s <e>\n", argv[0]);
        return 1;
    }
    size_t n = 1ULL << std::stoull(argv[1]);

    // init matrix and vector
    matrix A(generate_matrix(n, n, 0.8f), n, n);
    vector x = generate_vector(n, 0.8f);

    // perform actual product
    vector y = product(A, x);

    // for (size_t i = 0; i < n; i++)
    //     std::printf("%.6f\n", y[i]);

    return 0;
}
