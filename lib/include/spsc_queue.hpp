#ifndef SPSC_QUEUE_HPP
#define SPSC_QUEUE_HPP

#include <atomic>
#include <cassert>
#include <cstddef>
#include <optional>

namespace spm
{

template <typename T>
class spsc_queue
{
public:
    spsc_queue(size_t capacity = 1024)
        : m_capacity(capacity), m_head(0), m_tail(0), m_closed(false)
    {
        assert(capacity > 0);
        m_data = new T[capacity];
        m_ready = new std::atomic<bool>[capacity];
        for (size_t i = 0; i < capacity; i++)
            m_ready[i].store(true);
    }

    inline size_t capacity() const { return m_capacity; }

    inline size_t size() const
    {
        return (m_tail.load() - m_head.load()) % m_capacity;
    }

    void push(const T& value)
    {
        // book an index
        size_t tail = m_tail.fetch_add(1) % m_capacity;

        // loop until the slot is available
        while (!m_ready[tail].load(std::memory_order_acquire))
            ;

        // write the new value
        m_data[tail] = value;

        // publish the result
        m_ready[tail].store(false, std::memory_order_release);
    }

    void push(T&& value)
    {
        // book an index
        size_t tail = m_tail.fetch_add(1) % m_capacity;

        // loop until the slot is available
        while (!m_ready[tail].load(std::memory_order_acquire))
            ;

        // write the new value
        m_data[tail] = std::move(value);

        // publish the result
        m_ready[tail].store(false, std::memory_order_release);
    }

    std::optional<T> pop()
    {
        // book an index
        size_t head = m_head.fetch_add(1) % m_capacity;

        // loop until the slot is written and the queue is not closed
        while (m_ready[head].load(std::memory_order_acquire) &&
               !m_closed.load())
            ;

        if (!m_ready[head].load(std::memory_order_acquire))
        {
            // read the value
            T value = m_data[head];

            // make the slot available again
            m_ready[head].store(true, std::memory_order_release);

            return value;
        }
        else
        {
            // the queue is closed
            return std::nullopt;
        }
    }

    inline bool is_closed() const { return m_closed.load(); }

    void close() { m_closed.store(true); }

    ~spsc_queue()
    {
        close();
        delete[] m_data;
        delete[] m_ready;
    }

private:
    const size_t m_capacity;

    T* m_data;
    std::atomic<bool>* m_ready;

    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;

    std::atomic<bool> m_closed;
};

} // namespace spm

#endif
