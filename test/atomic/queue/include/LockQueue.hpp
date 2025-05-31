#ifndef LOCK_QUEUE_HPP
#define LOCK_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>

template <typename T, size_t Capacity = 1024>
class LockQueue
{
public:
    LockQueue() : m_Size(0), m_Head(0), m_Tail(0) {}

    inline size_t capacity() const { return Capacity; }

    inline size_t size() const { return m_Size; }

    void push(const T& value)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        while (m_Size >= Capacity)
            m_Full.wait(lock);

        m_Data[m_Tail] = value;
        m_Tail = (m_Tail + 1) % Capacity;
        m_Size++;

        m_Empty.notify_one();
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        while (m_Size <= 0)
            m_Empty.wait(lock);

        T value = m_Data[m_Head];
        m_Head = (m_Head + 1) % Capacity;
        m_Size--;

        m_Full.notify_one();

        return value;
    }

    ~LockQueue() {}

private:
    T m_Data[Capacity];
    size_t m_Size;
    size_t m_Head;
    size_t m_Tail;
    std::mutex m_Mutex;
    std::condition_variable m_Empty, m_Full;
};

#endif
