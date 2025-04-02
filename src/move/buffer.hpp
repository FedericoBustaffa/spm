#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

class buffer
{
public:
    // default constructor
    buffer(size_t capacity);

    // copy constructor
    buffer(const buffer &other);

    // move constructor
    buffer(buffer &&other) noexcept;

    // copy assign operator
    void operator=(const buffer &other);

    // move assign operator
    void operator=(buffer &&other) noexcept;

    ~buffer();

private:
    size_t m_size;
    size_t m_capacity;
    int *m_buffer;
};

#endif
