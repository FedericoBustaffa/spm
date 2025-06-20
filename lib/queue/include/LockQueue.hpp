#ifndef LOCK_QUEUE_HPP
#define LOCK_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>

template <typename T>
class LockQueue
{
public:
    LockQueue(size_t capacity = 1024)
        : m_Size(0), m_Capacity(capacity), m_Head(0), m_Tail(0), m_Closed(false)
    {
        m_Data = new T[capacity];
    }

    inline size_t capacity() const { return m_Capacity; }

    inline size_t size() const { return m_Size; }

    void push(T&& value)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (m_Closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_Size >= m_Capacity)
            m_Full.wait(lock);

        m_Data[m_Tail] = std::move(value);
        m_Tail = (m_Tail + 1) % m_Capacity;
        m_Size++;

        m_Empty.notify_one();
    }

    void push(const T& value)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        if (m_Closed)
            throw std::runtime_error("CLOSED QUEUE");

        while (m_Size >= m_Capacity)
            m_Full.wait(lock);

        m_Data[m_Tail] = value;
        m_Tail = (m_Tail + 1) % m_Capacity;
        m_Size++;

        m_Empty.notify_one();
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        while (m_Size <= 0 && !m_Closed)
            m_Empty.wait(lock);

        if (m_Size <= 0)
            return std::nullopt;

        T value = m_Data[m_Head];
        m_Head = (m_Head + 1) % m_Capacity;
        m_Size--;

        m_Full.notify_one();

        return value;
    }

    inline bool is_closed() const { return m_Closed; }

    void close()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Closed = true;
        m_Empty.notify_all();
    }

    ~LockQueue()
    {
        close();
        delete[] m_Data;
    }

private:
    T* m_Data;
    size_t m_Size;
    size_t m_Capacity;

    size_t m_Head;
    size_t m_Tail;

    std::mutex m_Mutex;
    std::condition_variable m_Empty, m_Full;
    bool m_Closed;
};

#endif
