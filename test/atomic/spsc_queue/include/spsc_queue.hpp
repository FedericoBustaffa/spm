#ifndef SPSC_QUEUE_HPP
#define SPSC_QUEUE_HPP

#include <cstddef>

template <typename T>
class spsc_queue
{
public:
    spsc_queue(size_t size) : m_size(size) {}

    inline size_t size() const { return m_size; }

    ~spsc_queue() {}

private:
    size_t m_size;
};

#endif // SPSC_QUEUE_HPP
