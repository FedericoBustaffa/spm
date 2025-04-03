#include <iostream>
#include <vector>
#include <random>
#include <cassert>

#include "timer.hpp"
#include "thread_pool.hpp"

std::vector<double> generate_vector(size_t n_elems)
{
	std::mt19937 rand_eng(42);
	std::normal_distribution<double> dist;
	std::vector<double> values;

	for (size_t i = 0; i < n_elems; i++)
		values.push_back(dist(rand_eng));

	return values;
}

double sum(double a, double b)
{
	return a + b;
}

template <typename T, typename Func, typename... Args>
std::vector<T> map(Func&& func, const std::vector<T>& v)
{
	std::vector<T> res;
	res.reserve(v.size());

	for (const auto& i : v)
		res.push_back(func(i));

	return res;
}

int main(int argc, const char** argv)
{
	size_t n = 10000;
	if (argc >= 2)
		n = std::atol(argv[1]);

	timer t;

	std::vector<double> v = generate_vector(n);

	auto partial_sum = std::bind(sum, 10.0, std::placeholders::_1);

	t.start();
	auto v1 = map(partial_sum, v);
	double stime = t.stop();
	std::cout << "sequential time: " << stime << " seconds" << std::endl;

	thread_pool pool;
	t.start();
	auto f = pool.map_async(partial_sum, v, n / pool.size());
	auto v2 = f.get();

	double ptime = t.stop();
	std::cout << "parallel time: " << ptime << " seconds" << std::endl;

	assert(std::equal(v1.begin(), v1.end(), v2.begin()));

	std::cout << "speed up: " << stime / ptime << std::endl;

	return 0;
}
