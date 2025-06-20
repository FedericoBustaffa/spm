#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace spm
{

template <typename T>
class lock_queue
{
public:
    /**
     * @brief Construct a new task queue object with the given capacity. If
     * capacity is equal to 0 the queue is unbounded.
     *
     * @param capacity the capacity of the queue.
     */
    lock_queue(size_t capacity) : m_closed(false), m_capacity(capacity) {}

    /**
     * @brief Returns the number of tasks in the queue.
     *
     */
    inline size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_buffer.size();
    }

    /**
     * @brief Check if the queue is empty or not.
     *
     * @return true if the size of the queue is equal to 0.
     * @return false otherwise
     */
    inline bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_buffer.size() == 0;
    }

    /**
     * @brief Check if the queue is full or not.
     *
     * @return true if the size of the queue is equal to its capacity.
     * @return false otherwise
     */
    inline bool full() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_capacity == 0 ? false : m_buffer.size() >= m_capacity;
    }

    /**
     * @brief Returns the capacity of the queue.
     *
     */
    inline size_t capacity() const { return m_capacity; }

    /**
     * @brief Push a task into the queue and returns a future to handle the
     * result. If the queue is full it blocks until the task is enqueued.
     *
     * @param func the function to execute.
     * @param args its arguments.
     * @return a future with the return value of the passed function.
     */
    void push(const T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_capacity == 0 ? false : m_buffer.size() >= m_capacity)
            m_full.wait(lock);

        m_buffer.emplace(value);
        m_empty.notify_one();
    }

    /**
     * @brief Push a task into the queue and returns a future to handle the
     * result. If the queue is full it blocks until the task is enqueued.
     *
     * @param func the function to execute.
     * @param args its arguments.
     * @return a future with the return value of the passed function.
     */
    void push(T&& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_capacity == 0 ? false : m_buffer.size() >= m_capacity)
            m_full.wait(lock);

        m_buffer.emplace(value);
        m_empty.notify_one();
    }

    /**
     * @brief Extract a task from the queue. If the queue is empty it blocks
     * until a new task is consumed.
     *
     * @return A void(void) function. It can be return nothing if the queue
     * is closed and tasks are in the queue.
     */
    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_buffer.empty() && !m_closed)
            m_empty.wait(lock);

        if (m_buffer.empty())
            return std::nullopt;

        T value = std::move(m_buffer.front());
        m_buffer.pop();

        m_full.notify_one();

        return value;
    }

    /**
     * @brief Check if the queue is closed or not.
     *
     * @return true if the queue is already closed.
     * @return false otherwise.
     */
    inline bool is_closed() const { return m_closed; }

    /**
     * @brief Close the queue and notifies all the other threads waiting on
     * `pop`.
     *
     */
    void close()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_closed = true;
        m_empty.notify_all();
    }

    /**
     * @brief Destroy the task queue object and calls `close` method.
     *
     */
    ~lock_queue() { close(); }

private:
    bool m_closed;
    const size_t m_capacity;
    std::queue<T> m_buffer;

    mutable std::mutex m_mutex;
    std::condition_variable m_empty, m_full;
};

} // namespace spm

#endif
