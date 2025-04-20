#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cassert>
#include <cstddef>

class Matrix
{
public:
    Matrix(size_t rows, size_t cols)
        : m_Rows(rows), m_Cols(cols), m_Matrix(nullptr)
    {
        m_Matrix = new double[rows * cols];
        for (size_t i = 0; i < rows * cols; i++)
            m_Matrix[i] = 0.0;
    }

    Matrix(Matrix&& other)
        : m_Rows(other.m_Rows), m_Cols(other.m_Cols), m_Matrix(other.m_Matrix)
    {
        other.m_Rows = 0;
        other.m_Cols = 0;
        other.m_Matrix = nullptr;
    }

    Matrix& operator=(Matrix&& other) noexcept
    {
        m_Rows = other.m_Rows;
        m_Cols = other.m_Cols;
        m_Matrix = other.m_Matrix;

        other.m_Rows = 0;
        other.m_Cols = 0;
        other.m_Matrix = nullptr;

        return *this;
    }

    inline size_t rows() const { return m_Rows; }

    inline size_t cols() const { return m_Cols; }

    const double& operator()(size_t i, size_t j) const
    {
        assert(i < m_Rows && j < m_Cols);
        return m_Matrix[i * m_Cols + j];
    }

    double& operator()(size_t i, size_t j)
    {
        assert(i < m_Rows && j < m_Cols);
        return m_Matrix[i * m_Cols + j];
    }

    Matrix transpose() const
    {
        Matrix t(m_Rows, m_Cols);
        for (size_t i = 0; i < m_Rows; i++)
            for (size_t j = 0; j < m_Cols; j++)
                t(j, i) = m_Matrix[i * m_Cols + j];

        return t;
    }

    ~Matrix() { delete[] m_Matrix; }

private:
    size_t m_Rows, m_Cols;
    double* m_Matrix;
};

#endif
