#include "buffer.hpp"

#include <iostream>

buffer::buffer(size_t capacity) : m_size(0), m_capacity(capacity)
{
    m_buffer = new int[capacity];
    std::cout << "default constructor" << std::endl;
}

// copy constructor
buffer::buffer(const buffer& other)
    : m_size(other.m_size), m_capacity(other.m_capacity)
{
    m_buffer = new int[m_capacity];
    for (size_t i = 0; i < m_size; i++)
        m_buffer[i] = other.m_buffer[i];

    std::cout << "copy constructor" << std::endl;
}

// move constructor
buffer::buffer(buffer&& other) noexcept
    : m_size(other.m_size), m_capacity(other.m_capacity)
{
    m_buffer = other.m_buffer;

    other.m_size = 0;
    other.m_capacity = 0;
    other.m_buffer = nullptr;

    std::cout << "move constructor" << std::endl;
}

// copy assign operator
void buffer::operator=(const buffer& other)
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;

    delete[] m_buffer;
    m_buffer = new int[m_capacity];

    for (size_t i = 0; i < m_size; i++)
        m_buffer[i] = other.m_buffer[i];

    std::cout << "copy assign operator" << std::endl;
}

// move assign operator
void buffer::operator=(buffer&& other) noexcept
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    delete[] m_buffer;
    m_buffer = other.m_buffer;

    other.m_size = 0;
    other.m_capacity = 0;
    other.m_buffer = nullptr;

    std::cout << "move assign operator" << std::endl;
}

// destructor
buffer::~buffer() { delete[] m_buffer; }
