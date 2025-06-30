#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <vector>

class matrix
{
public:
    matrix(double* raw, size_t rows, size_t cols)
    {
        m_values.reserve(rows * cols);
        m_rows.reserve(rows * cols);
        m_cols.reserve(rows * cols);

        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < rows; j++)
            {
                if (raw[i * rows + j] != 0.0)
                {
                    m_values.push_back(raw[i * rows + j]);
                    m_rows.push_back(i);
                    m_cols.push_back(j);
                }
            }
        }

        m_values.shrink_to_fit();
        m_rows.shrink_to_fit();
        m_cols.shrink_to_fit();

        delete[] raw;
    }

    inline size_t size() const { return m_values.size(); }

    inline size_t row(size_t i) const { return m_rows[i]; }

    inline size_t col(size_t i) const { return m_cols[i]; }

    inline double operator[](size_t i) const { return m_values[i]; }

    ~matrix() {}

private:
    std::vector<double> m_values;
    std::vector<size_t> m_rows;
    std::vector<size_t> m_cols;
};

#endif