#include "benchmarks.hpp"

std::vector<int> sequential(const std::vector<int>& numbers)
{
    std::vector<int> out;
    out.reserve(numbers.size());

    for (size_t i = 0; i < numbers.size(); i++)
        out.push_back(fibonacci(numbers[i]));

    return out;
}
