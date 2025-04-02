#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "hpc_helpers.hpp"

inline float max_unroll2(const float *input, size_t K)
{
    float max0 = -std::numeric_limits<float>::infinity();
    float max1 = -std::numeric_limits<float>::infinity();

    size_t i = 0;
#pragma GCC unroll 2
    for (; i + 1 < K; i += 2)
    {
        max0 = std::max(max0, input[i]);
        max1 = std::max(max1, input[i + 1]);
    }

    for (; i < K; ++i)
        max0 = std::max(max0, input[i]);

    return std::max(max0, max1);
}

inline float max_unroll4(const float *input, size_t K)
{
    // Find the maximum to stabilize the computation of the exponential
    float max0 = -std::numeric_limits<float>::infinity();
    float max1 = -std::numeric_limits<float>::infinity();
    float max2 = -std::numeric_limits<float>::infinity();
    float max3 = -std::numeric_limits<float>::infinity();

    size_t i = 0;
#pragma GCC unroll 4
    for (; i + 3 < K; i += 4)
    {
        max0 = std::max(max0, input[i]);
        max1 = std::max(max1, input[i + 1]);
        max2 = std::max(max2, input[i + 2]);
        max3 = std::max(max3, input[i + 3]);
    }

    for (; i < K; ++i)
        max0 = std::max(max0, input[i]);

    return std::max(std::max(max0, max1), std::max(max2, max3));
}

void softmax_auto(const float *__restrict__ input, float *__restrict__ output, size_t K)
{
#if 0
	float max_val = -std::numeric_limits<float>::infinity();
	for (size_t i = 0; i < K; ++i)
		max_val = std::max(max_val, input[i]);
#else
#if 0
	float max_val = max_unroll2(input, K);
#else
    float max_val = max_unroll4(input, K);
#endif
#endif

    // computes all exponentials with the shift of max_val and the total sum
    float sum = 0.0f;
#if 0
	for (size_t i = 0; i < K; ++i)
	{
		output[i] = std::exp(input[i] - max_val);
		sum += output[i];
	}
#else
#pragma GCC ivdep
    for (size_t i = 0; i < K; ++i)
        output[i] = std::exp(input[i] - max_val);

#pragma GCC ivdep
    for (size_t i = 0; i < K; ++i)
        sum += output[i];
#endif

#if 0
	for (size_t i = 0; i < K; ++i)
		output[i] /= sum;
#else
    float inv_sum = 1.0f / sum;
    for (size_t i = 0; i < K; ++i)
        output[i] *= inv_sum;
#endif
}

std::vector<float> generate_random_input(size_t K, float min = -1.0f, float max = 1.0f)
{
    std::vector<float> input(K);
    // std::random_device rd;
    // std::mt19937 gen(rd());
    std::mt19937 gen(5489); // fixed seed for reproducible results
    std::uniform_real_distribution<float> dis(min, max);
    for (size_t i = 0; i < K; ++i)
    {
        input[i] = dis(gen);
    }
    return input;
}

void printResult(std::vector<float> &v, size_t K)
{
    for (size_t i = 0; i < K; ++i)
    {
        std::fprintf(stderr, "%f\n", v[i]);
    }
}

int main(int argc, char *argv[])
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

    TIMERSTART(softime_auto);
    softmax_auto(input.data(), output.data(), K);
    TIMERSTOP(softime_auto);

    // print the results on the standard output
    if (print)
    {
        printResult(output, K);
    }
}
