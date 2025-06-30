#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>

class vector
{
public:
    vector(double* raw, size_t length)
    {
        m_values.reserve(length);
        m_indices.reserve(length);

        for (size_t i = 0; i < length; i++)
        {
            if (raw[i] != 0.0)
            {
                m_values.push_back(raw[i]);
                m_indices.push_back(i);
            }
        }

        m_values.shrink_to_fit();
        m_indices.shrink_to_fit();

        delete[] raw;
    }

    ~vector() {}

private:
    std::vector<size_t> m_values;
    std::vector<size_t> m_indices;
};

#endif