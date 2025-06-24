#ifndef LOCK_QUEUE_HPP
#define LOCK_QUEUE_HPP

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace spm
{

template <typename T>
class lock_queue
{
public:
    lock_queue(size_t capacity = 1024)
        : m_size(0), m_capacity(capacity), m_head(0), m_tail(0), m_closed(false)
    {
        assert(capacity > 0);
        m_data = new T[capacity];
    }

    inline size_t capacity() const { return m_capacity; }

    inline size_t size() const { return m_size; }

    void push(T&& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_size >= m_capacity)
            m_full.wait(lock);

        m_data[m_tail] = std::move(value);
        m_tail = (m_tail + 1) % m_capacity;
        m_size++;

        m_empty.notify_one();
    }

    void push(const T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_size >= m_capacity)
            m_full.wait(lock);

        m_data[m_tail] = value;
        m_tail = (m_tail + 1) % m_capacity;
        m_size++;

        m_empty.notify_one();
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_size <= 0 && !m_closed)
            m_empty.wait(lock);

        if (m_size <= 0)
            return std::nullopt;

        T value = m_data[m_head];
        m_head = (m_head + 1) % m_capacity;
        m_size--;

        m_full.notify_one();

        return value;
    }

    inline bool is_closed() const { return m_closed; }

    void close()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_closed = true;
        m_empty.notify_all();
    }

    ~lock_queue()
    {
        close();
        delete[] m_data;
    }

private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

    size_t m_head;
    size_t m_tail;

    std::mutex m_mutex;
    std::condition_variable m_empty, m_full;
    bool m_closed;
};

} // namespace spm

#endif
