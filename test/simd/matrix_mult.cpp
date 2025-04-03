#include <cassert>
#include <immintrin.h>
#include <iostream>
#include <random>

#include "matrix.hpp"
#include "timer.hpp"

void init(matrix &m)
{
    std::mt19937 engine(42);
    std::uniform_real_distribution<float> dist(0.0f, 10.0f);
    for (size_t i = 0; i < m.rows(); i++)
        for (size_t j = 0; j < m.cols(); j++)
            m(i, j) = dist(engine);
}

matrix naive_mm(const matrix &a, const matrix &b)
{
    assert(a.cols() == b.rows());
    matrix c(a.rows(), b.cols());

    for (size_t i = 0; i < a.rows(); ++i)
    {
        for (size_t j = 0; j < b.cols(); ++j)
        {
            for (size_t k = 0; k < a.cols(); ++k)
                c(i, j) += a(i, k) * b(k, j);
        }
    }

    return c;
}

matrix transpose_mm(const matrix &a, const matrix &b)
{
    assert(a.cols() == b.rows());
    matrix res(a.rows(), b.cols());
    matrix bt = b.transpose();
    for (size_t i = 0; i < a.rows(); i++)
    {
        for (size_t j = 0; j < bt.rows(); j++)
        {
            for (size_t k = 0; k < a.cols(); k++)
                res(i, j) += a(i, k) * bt(j, k);
        }
    }

    return res;
}

inline float hsum_sse3(__m128 v)
{
    __m128 shuf = _mm_movehdup_ps(v);  // broadcast elements 3,1 to 2,0
    __m128 maxs = _mm_add_ps(v, shuf); // adds the four SP FP values
    shuf = _mm_movehl_ps(shuf, maxs);  // moves the upper two SP FP values of shuf to the lower two
                                       // SP FP values of the result.

    // adds the lower SP FP values of maxs and shuf; the upper three
    // SP FP values are passed through from maxs
    maxs = _mm_add_ss(maxs, shuf);

    // extract a single-precision floating-point value the first vector element
    return _mm_cvtss_f32(maxs);
}

inline float hsum_avx(__m256 v)
{
    __m128 lo = _mm256_castps256_ps128(v);   // low 128
    __m128 hi = _mm256_extractf128_ps(v, 1); // high 128
    lo = _mm_add_ps(lo, hi);                 // max the low 128

    return hsum_sse3(lo); // and inline the sse3 version
}

matrix avx_mm(const matrix &a, const matrix &b)
{
    assert(a.cols() == b.rows());
    matrix c(a.rows(), b.cols());
    matrix bt = b.transpose();

    for (size_t i = 0; i < a.rows(); ++i)
    {
        for (size_t j = 0; j < bt.rows(); ++j)
        {
            __m256 vc = _mm256_setzero_ps();
            for (size_t k = 0; k < a.cols(); k += 8)
            {
                __m256 va = _mm256_load_ps(&a(i, k));
                __m256 vb = _mm256_load_ps(&bt(j, k));
                vc = _mm256_fmadd_ps(va, vb, vc);
            }
            c(i, j) = hsum_avx(vc);
        }
    }

    return c;
}

int main(int argc, const char **argv)
{
    size_t n = 512;
    if (argc == 2)
        n = std::atoi(argv[1]);

    matrix a(n, n);
    matrix b(n, n);
    init(a);
    init(b);

    timer timer;
    auto c = naive_mm(a, b);
    double naive_time = timer.lap();
    std::printf("naive time: %f\n", naive_time);
    c.save("naive.txt");

    timer.reset();
    auto c2 = transpose_mm(a, b);
    double transpose_time = timer.lap();
    std::printf("-----\ntranspose time: %f\n", transpose_time);
    std::printf("speed_up over naive: %.2f\n", naive_time / transpose_time);
    c2.save("transpose.txt");

    timer.reset();
    auto c3 = avx_mm(a, b);
    double avx_time = timer.lap();
    std::printf("-----\navx time: %f\n", avx_time);
    std::printf("speed_up over naive: %.2f\n", naive_time / avx_time);
    std::printf("speed_up over transpose: %.2f\n", transpose_time / avx_time);
    c3.save("avx.txt");

    return 0;
}
