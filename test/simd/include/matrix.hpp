#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <fstream>
#include <immintrin.h>

class matrix
{
public:
	matrix(size_t rows, size_t cols) : m_Rows(rows), m_Cols(cols)
	{
		m_Matrix = static_cast<float*>(_mm_malloc(rows * cols * sizeof(float), 32));
		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
				m_Matrix[i * cols + j] = 0.0f;
		}
	}

	matrix(const matrix& other) : m_Rows(other.m_Rows), m_Cols(other.m_Cols)
	{
		m_Matrix = static_cast<float*>(_mm_malloc(m_Rows * m_Cols * sizeof(float), 32));
		for (size_t i = 0; i < m_Rows; i++)
		{
			for (size_t j = 0; j < m_Cols; j++)
				m_Matrix[i * m_Cols + j] = other.m_Matrix[i * m_Cols + j];
		}
	}

	~matrix() { _mm_free(m_Matrix); }

	void operator=(const matrix& other)
	{
		m_Matrix = static_cast<float*>(_mm_malloc(m_Rows * m_Cols * sizeof(float), 32));
		for (size_t i = 0; i < m_Rows; i++)
		{
			for (size_t j = 0; j < m_Cols; j++)
				m_Matrix[i * m_Cols + j] = other.m_Matrix[i * m_Cols + j];
		}
	}

	size_t rows() const { return m_Rows; }
	size_t cols() const { return m_Cols; }

	float& operator()(size_t i, size_t j) { return m_Matrix[i * m_Cols + j]; }
	float& operator()(size_t i, size_t j) const { return m_Matrix[i * m_Cols + j]; }

	matrix transpose() const
	{
		matrix t(m_Cols, m_Rows);
		for (size_t i = 0; i < m_Rows; i++)
		{
			for (size_t j = 0; j < m_Cols; j++)
				t(j, i) = m_Matrix[i * m_Cols + j];
		}

		return t;
	}

	void save(const char* path)
	{
		std::ofstream file(path);
		for (size_t i = 0; i < m_Rows; i++)
		{
			for (size_t j = 0; j < m_Cols; j++)
				file << m_Matrix[i * m_Cols + j] << " ";
			file << std::endl;
		}
	}

private:
	size_t m_Rows, m_Cols;
	float* m_Matrix;
};

#endif