#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <immintrin.h>
#include <random>

void init(float* a, float* b, float* c, uint64_t n)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0, 10.0);

	for (size_t i = 0; i < n; i++)
	{
		a[i] = dis(gen);
		b[i] = dis(gen);
		c[i] = dis(gen);
	}
}

void mult_add(float* a, float* b, float* c, float* res, size_t n)
{
	for (size_t i = 0; i < n; i++)
		res[i] = a[i] * b[i] + c[i];
}

void fuse_mult_add(float* a, float* b, float* c, float* res, size_t n)
{
	for (size_t i = 0; i < n; i += 8)
	{
		__m256 va = _mm256_load_ps(&a[i]);
		__m256 vb = _mm256_load_ps(&b[i]);
		__m256 vc = _mm256_load_ps(&c[i]);
		__m256 vs = _mm256_fmadd_ps(va, vb, vc);
		_mm256_store_ps(&res[i], vs);
	}
}

void compare(float* a, float* b)
{
	uint32_t mismatch = 0;
	for (size_t i = 0; i < 8; i++)
	{
		if (std::fabs(a[i] - b[i]) > 1e-6) // fma is more precise than plain
		{
			mismatch++;
			std::printf("mismatch at index %lu: %.6f != %.6f\n", i, a[i], b[i]);
		}
	}
	if (!mismatch)
		std::cout << "Arrays are equal." << std::endl;
	else
		std::cout << mismatch << " mismatches" << std::endl;
}

int main(int argc, const char** argv)
{
	size_t k = 1UL << 20;
	if (argc > 1)
		k = 1UL << std::atoi(argv[1]);

	float* a = static_cast<float*>(_mm_malloc(k * sizeof(float), 32));
	float* b = static_cast<float*>(_mm_malloc(k * sizeof(float), 32));
	float* c = static_cast<float*>(_mm_malloc(k * sizeof(float), 32));
	float* res1 = static_cast<float*>(_mm_malloc(k * sizeof(float), 32));
	float* res2 = static_cast<float*>(_mm_malloc(k * sizeof(float), 32));

	init(a, b, c, k);

	auto start = std::chrono::high_resolution_clock::now();
	mult_add(a, b, c, res1, k);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::printf("plain mult add time: %f\n", duration.count());

	start = std::chrono::high_resolution_clock::now();
	fuse_mult_add(a, b, c, res2, k);
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration2 = end - start;
	std::printf("fused mult add time: %f\n", duration2.count());

	std::printf("speed up: %f\n", duration.count() / duration2.count());

	compare(res1, res2);

	_mm_free(a);
	_mm_free(b);
	_mm_free(c);
	_mm_free(res1);
	_mm_free(res2);

	return 0;
}
