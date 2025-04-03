#include <cstdio>
#include <immintrin.h>
#include <random>

#include "timer.hpp"

void init(float *v, size_t n)
{
    std::mt19937 engine(42);
    std::uniform_real_distribution<float> dist(0.0f, 10.0f);
    for (size_t i = 0; i < n; i++)
        v[i] = dist(engine);
}

void transform_scalar(float *in, float *out, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        if (in[i] < 0.0f)
            out[i] = in[i] * 2.0f;
        else
            out[i] = in[i] / 2.0f;
    }
}

void transform_scalar_avx(float *in, float *out, size_t n)
{
    __m256 twos = _mm256_set1_ps(2.0f);
    __m256 zeros = _mm256_set1_ps(0.0f);
    for (size_t i = 0; i < n; i += 8)
    {
        __m256 v = _mm256_load_ps(&in[i]);
        __m256 mul = _mm256_mul_ps(v, twos);
        __m256 div = _mm256_div_ps(v, twos);

        __m256 mask = _mm256_cmp_ps(v, zeros, _CMP_LT_OS);
        __m256 blend = _mm256_blendv_ps(mul, div, mask);
        _mm256_store_ps(&out[i], blend);
    }
}

int main(int argc, const char **argv)
{
    size_t e = 20;
    if (argc > 1)
        e = std::atoi(argv[1]);

    size_t n = 1UL << e;

    float *in = static_cast<float *>(_mm_malloc(n * sizeof(float), 32));
    float *out = static_cast<float *>(_mm_malloc(n * sizeof(float), 32));

    init(in, n);
    std::fill(out, &out[n - 1], 0.0f);

    timer timer;
    transform_scalar(in, out, n);
    double plain_time = timer.lap();
    std::printf("plain time: %f\n", plain_time);

    timer.reset();
    transform_scalar_avx(in, out, n);
    double avx_time = timer.lap();
    std::printf("avx time: %f\n", avx_time);
    std::printf("avx speed_up over plain: %f\n", plain_time / avx_time);

    _mm_free(in);
    _mm_free(out);

    return 0;
}
