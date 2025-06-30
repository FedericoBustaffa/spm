#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <random>

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

double* generate_vector(size_t length, float sparsity)
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    double* data = new double[length];
    for (size_t i = 0; i < length; i++)
    {
        if (dist(engine) <= sparsity)
            data[i] = 0.0;
        else
            data[i] = dist(engine);
    }

    return data;
}

#endif