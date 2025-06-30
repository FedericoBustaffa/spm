#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cassert>
#include <cstring>
#include <vector>

class vector
{
public:
    vector(size_t length) : m_length(length)
    {
        m_data = new double[length];
        std::memset(m_data, 0, m_length);
    }

    inline size_t length() const { return m_length; }

    inline double operator[](size_t i) const
    {
        assert(i < m_length);
        return m_data[i];
    }

    inline double& operator[](size_t i)
    {
        assert(i < m_length);
        return m_data[i];
    }

    ~vector() { delete[] m_data; }

private:
    size_t m_length;
    double* m_data;
};

#endif