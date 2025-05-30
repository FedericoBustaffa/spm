#ifndef ATOMIC_QUEUE_HPP
#define ATOMIC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <optional>

template <typename T>
class atomic_queue
{
public:
    atomic_queue(size_t size)
        : m_data(nullptr), m_size(size), m_head(0), m_tail(0)
    {
        m_data = new T[size];
    }

    inline size_t size() const { return m_size; }

    void push(const T& value)
    {
        m_data[m_tail.load()] = value;
        m_tail.store((m_tail + 1) % m_size);
    }

    std::optional<T> pop() {}

    ~atomic_queue() { delete[] m_data; }

private:
    T* m_data;
    size_t m_size;
    std::atomic<size_t> m_head, m_tail;
};

#endif // ATOMIC_QUEUE_HPP
