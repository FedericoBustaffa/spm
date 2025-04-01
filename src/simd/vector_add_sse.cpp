#include <iostream>
#include <random>
#include <chrono>
#include <immintrin.h>

void init(float* a, float* b, uint64_t n)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0, 1.0);

	for (size_t i = 0; i < n; i++)
	{
		a[i] = dis(gen);
		b[i] = dis(gen);
	}
}

void copy(float* a, float* b, uint64_t n)
{
	for (size_t i = 0; i < n; i++)
		a[i] = b[i];
}

void vadd(float* a, float* b, float* c, uint64_t n)
{
	for (size_t i = 0; i < n; i++)
		c[i] = a[i] + b[i];
}

void vadd128(float* a, float* b, float* c, uint64_t n)
{
	for (uint64_t i = 0; i < n; i += 4)
	{
		__m128 va = _mm_loadu_ps(&a[i]);
		__m128 vb = _mm_loadu_ps(&b[i]);
		__m128 vs = _mm_add_ps(va, vb);
		_mm_storeu_ps(&c[i], vs);
	}
}

void vadd128_aligned(float* a, float* b, float* c, uint64_t n)
{
	for (uint64_t i = 0; i < n; i += 4)
	{
		__m128 va = _mm_load_ps(&a[i]);
		__m128 vb = _mm_load_ps(&b[i]);
		__m128 vs = _mm_add_ps(va, vb);
		_mm_store_ps(&c[i], vs);
	}
}

int main(int argc, const char** argv)
{
	uint64_t e = argc <= 1 ? 20UL : std::atoi(argv[1]);
	uint64_t n = 1 << e;
	double size = (n * sizeof(float)) / (1024 * 1024);
	std::printf("array size: %lu -> %g MB\n", n, size);

	float* a = new float[n];
	float* b = new float[n];
	float* c = new float[n];

	auto start = std::chrono::high_resolution_clock::now();
	init(a, b, n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> init_time = end - start;
	std::printf("initialization elapsed time: %f seconds\n", init_time.count());

	start = std::chrono::high_resolution_clock::now();
	vadd(a, b, c, n);
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> plain = end - start;
	std::printf("plain elapsed time: %f seconds\n", plain.count());

	start = std::chrono::high_resolution_clock::now();
	vadd128(a, b, c, n);
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> sse = end - start;
	std::printf("sse unaligned elapsed time: %f seconds\n", sse.count());

	float* a2 = static_cast<float*>(_mm_malloc(n * sizeof(float), 16));
	float* b2 = static_cast<float*>(_mm_malloc(n * sizeof(float), 16));
	float* c2 = static_cast<float*>(_mm_malloc(n * sizeof(float), 16));
	init(a2, b2, n);

	start = std::chrono::high_resolution_clock::now();
	vadd(a2, b2, c2, n);
	end = std::chrono::high_resolution_clock::now();
	plain = end - start;
	std::printf("plain aligned elapsed time: %f seconds\n", plain.count());

	start = std::chrono::high_resolution_clock::now();
	vadd128_aligned(a2, b2, c2, n);
	end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> sse_aligned = end - start;
	std::printf("sse aligned elapsed time: %f seconds\n", sse_aligned.count());

	std::printf("plain to sse speed-up: %f\n", plain.count() / sse.count());
	std::printf("plain to sse aligned speed-up: %f\n", plain.count() / sse_aligned.count());

	delete[] a;
	delete[] b;
	delete[] c;

	_mm_free(a2);
	_mm_free(b2);
	_mm_free(c2);

	return 0;
}
