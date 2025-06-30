#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <random>

#include "vector.hpp"

double* generate_matrix(size_t rows, size_t cols, float sparsity)
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    double* data = new double[rows * cols];
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < rows; j++)
        {
            if (dist(engine) <= sparsity)
                data[i * rows + j] = 0.0;
            else
                data[i * rows + j] = dist(engine);
        }
    }

    return data;
}

vector generate_vector(size_t length, float sparsity)
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    vector v(length);
    for (size_t i = 0; i < length; i++)
        v[i] = dist(engine) <= sparsity ? 0.0 : dist(engine);

    return v;
}

#endif