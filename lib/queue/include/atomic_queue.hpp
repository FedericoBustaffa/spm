#ifndef ATOMIC_QUEUE_HPP
#define ATOMIC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <optional>

namespace spm
{

template <typename T>
class atomic_queue
{
public:
    atomic_queue(size_t capacity = 1024)
        : m_size(0), m_capacity(capacity), m_head(0), m_tail(0), m_closed(false)
    {
        m_data = new T[capacity];
    }

    inline size_t capacity() const { return m_capacity.load(); }

    inline size_t size() const { return m_size.load(); }

    void push(const T& value)
    {
        m_data[m_tail.load()] = value;
        m_tail.store((m_tail.load() + 1) % m_capacity);
        m_size.fetch_add(1);
    }

    void push(T&& value)
    {
        m_data[m_tail.load()] = std::move(value);
        m_tail.store((m_tail.load() + 1) % m_capacity);
        m_size.fetch_add(1);
    }

    std::optional<T> pop()
    {
        const T& value = m_data[m_head.load()];
        m_head.store((m_head.load() + 1) % m_capacity);
        m_size.fetch_sub(1);

        return value;
    }

    inline bool is_closed() const { return m_closed.load(); }

    void close() { m_closed.store(true); }

    ~atomic_queue()
    {
        close();
        delete[] m_data;
    }

private:
    T* m_data;
    std::atomic<size_t> m_size;
    std::atomic<size_t> m_capacity;

    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;

    std::atomic<bool> m_closed;
};

} // namespace spm

#endif
