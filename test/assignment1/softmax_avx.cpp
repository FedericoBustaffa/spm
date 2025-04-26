#include <algorithm>
#include <random>

#include "avx_mathfun.h"
#include "hpc_helpers.hpp"

float max_avx(const float* input, size_t K)
{
    int8_t carry = K % 8;

    // compute the max
    __m256 vmax = _mm256_loadu_ps(&input[0]);
    for (size_t i = 8; i < K - carry; i += 8)
    {
        __m256 v = _mm256_loadu_ps(&input[i]);
        vmax = _mm256_max_ps(v, vmax);
    }

    // reduce the vmax vector to 4 floats
    __m128 lo = _mm256_castps256_ps128(vmax);
    __m128 hi = _mm256_extractf128_ps(vmax, 1);
    lo = _mm_max_ps(lo, hi);

    // reduce the lo vector to 2 floats
    __m128 shuf = _mm_movehdup_ps(lo);
    lo = _mm_max_ps(lo, shuf);

    // extract the max from the last 2 elements
    shuf = _mm_movehl_ps(shuf, lo);
    lo = _mm_max_ps(lo, shuf);
    float max_val = _mm_cvtss_f32(lo);

    // handle last elements sequentially
    for (size_t i = K - carry; i < K; i++)
        max_val = std::max(max_val, input[i]);

    return max_val;
}

float hsum256_ps(__m256 v)
{
    // reduce the vmax vector to 4 floats
    __m128 lo = _mm256_castps256_ps128(v);
    __m128 hi = _mm256_extractf128_ps(v, 1);
    lo = _mm_add_ps(lo, hi);

    __m128 shuf = _mm_movehdup_ps(lo);
    __m128 maxs = _mm_add_ps(lo, shuf);
    shuf = _mm_movehl_ps(shuf, maxs);
    maxs = _mm_add_ss(maxs, shuf);

    return _mm_cvtss_f32(maxs);
}

float expsum_avx(const float* input, float* output, size_t K, float max_val)
{
    __m256 vsum = _mm256_setzero_ps();
    __m256 vmax = _mm256_set1_ps(max_val);
    int8_t carry = K % 8;
    for (size_t i = 0; i < K - carry; i += 8)
    {
        __m256 vin = _mm256_loadu_ps(&input[i]);
        __m256 e = exp256_ps(_mm256_sub_ps(vin, vmax));
        _mm256_storeu_ps(&output[i], e);

        vsum = _mm256_add_ps(vsum, e);
    }

    // horizontal sum of 8 elements vector
    float sum = hsum256_ps(vsum);

    // handle last elements sequentially
    for (size_t i = K - carry; i < K; i++)
    {
        output[i] = std::exp(input[i] - max_val);
        sum += output[i];
    }

    return sum;
}

void div_avx(float* output, size_t K, float sum)
{
    int8_t carry = K % 8;
    __m256 vsum = _mm256_set1_ps(sum);
    for (size_t i = 0; i < K - carry; i += 8)
    {
        __m256 v = _mm256_loadu_ps(&output[i]);
        v = _mm256_div_ps(v, vsum);
        _mm256_storeu_ps(&output[i], v);
    }

    // handle last elements sequentially
    for (size_t i = K - carry; i < K; ++i)
        output[i] /= sum;
}

void softmax_avx(const float* input, float* output, size_t K)
{
    // Find the maximum to stabilize the computation of the exponential
    float max_val = max_avx(input, K);

    // computes all exponentials with the shift of max_val and the total sum
    float sum = expsum_avx(input, output, K, max_val);

    // normalize by dividing for the total sum
    div_avx(output, K, sum);
}

std::vector<float> generate_random_input(size_t K, float min = -1.0f,
                                         float max = 1.0f)
{
    std::vector<float> input(K);
    std::mt19937 gen(5489); // fixed seed for reproducible results
    std::uniform_real_distribution<float> dis(min, max);
    for (size_t i = 0; i < K; ++i)
    {
        input[i] = dis(gen);
    }
    return input;
}

void printResult(std::vector<float>& v, size_t K)
{
    for (size_t i = 0; i < K; ++i)
    {
        std::fprintf(stderr, "%f\n", v[i]);
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::printf("use: %s K [1]\n", argv[0]);
        return 0;
    }
    size_t K = 0;
    if (argc >= 2)
    {
        K = std::stol(argv[1]);
    }
    bool print = false;
    if (argc == 3)
    {
        print = true;
    }
    std::vector<float> input = generate_random_input(K);
    std::vector<float> output(K);

    TIMERSTART(softime_avx);
    softmax_avx(input.data(), output.data(), K);
    TIMERSTOP(softime_avx);

    // print the results on the standard output
    if (print)
    {
        printResult(output, K);
    }
}
