#ifndef MPMC_QUEUE_HPP
#define MPMC_QUEUE_HPP

#include <atomic>
#include <cassert>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <thread>

namespace spm
{

template <typename T>
class mpmc_queue
{
public:
    /**
     * @brief Construct a bounded multi-producer/multi-consumer lock-free queue
     * with the given capacity. The capacity must be at least 1 or an assert
     * will fire.
     *
     * @param capacity number of slots available in the queue.
     */
    mpmc_queue(size_t capacity = 1024)
        : m_capacity(capacity), m_head(0), m_tail(0), m_closed(false)
    {
        assert(capacity > 0);
        m_data = new T[capacity];
        m_versions = new std::atomic<size_t>[capacity];
        for (size_t i = 0; i < capacity; i++)
            m_versions[i].store(i, std::memory_order_relaxed);
    }

    /**
     * @brief return the capacity of the queue.
     */
    inline size_t capacity() const { return m_capacity; }

    /**
     * @brief return the number of valid elements in the queue but it is not
     * really atomic and thread safe. Can be used to have an approximation.
     */
    inline size_t size() const
    {
        return (m_tail.load(std::memory_order_relaxed) -
                m_head.load(std::memory_order_relaxed)) %
               m_capacity;
    }

    /**
     * @brief push an element inside the queue (by value) if it is not full. If
     * so retries until a new slot is available.
     *
     * @param value the value that will be pushed into the queue by copy.
     *
     * @throw std::runtime_error if the method is invoked after the queue is
     * already closed.
     */
    void push(const T& value)
    {
        if (m_closed.load(std::memory_order_acquire))
            throw std::runtime_error("CLOSED QUEUE");

        // book an index
        size_t tail = m_tail.fetch_add(1, std::memory_order_relaxed);
        size_t index = tail % m_capacity;

        // loop until the slot is available
        while (m_versions[index].load(std::memory_order_acquire) != tail)
            std::this_thread::yield();

        // write the new value
        m_data[index] = value;

        // publish the result
        m_versions[index].store(tail + 1, std::memory_order_release);
    }

    /**
     * @brief push an element inside the queue by moving it if the queue is not
     * full. If so retries until a new slot is available.
     *
     * @param value the value that will be pushed into the queue by copy.
     *
     * @throw std::runtime_error if the method is invoked after the queue is
     * already closed.
     */
    void push(T&& value)
    {
        if (m_closed.load(std::memory_order_acquire))
            throw std::runtime_error("CLOSED QUEUE");

        // book an index
        size_t tail = m_tail.fetch_add(1, std::memory_order_relaxed);
        size_t index = tail % m_capacity;

        // loop until the slot is available
        while (m_versions[index].load(std::memory_order_acquire) != tail)
            std::this_thread::yield();

        // write the new value
        m_data[index] = std::move(value);

        // publish the result
        m_versions[index].store(tail + 1, std::memory_order_release);
    }

    /**
     * @brief remove and return an element from the queue. If the queue is empty
     * retry until a new value is pushed.
     *
     * @return a value of type `T` or a `std::nullopt`. The `std::nullopt` is
     * used to handle the case in which a thread waits on the pop and has to be
     * awakaned to terminate.
     *
     */
    std::optional<T> pop()
    {
        // book an index
        size_t head = m_head.fetch_add(1, std::memory_order_relaxed);
        size_t index = head % m_capacity;
        size_t version;

        // loop until the slot is readable or the queue is closed
        while (true)
        {
            version = m_versions[index].load(std::memory_order_acquire);
            if (version == head + 1)
                break;

            if (m_closed.load(std::memory_order_acquire))
                return std::nullopt;

            std::this_thread::yield();
        }

        // read the value
        T value = m_data[index];

        // make the slot available again
        m_versions[index].store(head + m_capacity, std::memory_order_release);

        return value;
    }

    /**
     * @brief returns the status of the queue.
     */
    inline bool is_closed() const
    {
        return m_closed.load(std::memory_order_relaxed);
    }

    /**
     * @brief close the queue, preventing further entries and enabling threads
     * to pop a `std::nullopt` value if there are no more valid values in the
     * queue.
     */
    void close() { m_closed.store(true, std::memory_order_relaxed); }

    /**
     * @brief automatically close the queue and free memory.
     */
    ~mpmc_queue()
    {
        close();
        delete[] m_data;
        delete[] m_versions;
    }

private:
    const size_t m_capacity;

    T* m_data;
    std::atomic<size_t>* m_versions;

    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;

    std::atomic<bool> m_closed;
};

} // namespace spm

#endif
