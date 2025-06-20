#ifndef ATOMIC_QUEUE_HPP
#define ATOMIC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <optional>

template <typename T, size_t Capacity = 1024>
class AtomicQueue
{
public:
    AtomicQueue() : m_Size(0), m_Head(0), m_Tail(0) {}

    inline size_t capacity() const { return Capacity; }

    inline size_t size() const { return m_Size; }

    void push(const T& value)
    {
        m_Data[m_Tail.load()] = value;
        m_Tail.store((m_Tail.load() + 1) % Capacity);
        m_Size.fetch_add(1);
    }

    std::optional<T> pop()
    {
        const T& value = m_Data[m_Head.load()];
        m_Head.store((m_Head.load() + 1) % Capacity);
        m_Size.fetch_sub(1);

        return value;
    }

    ~AtomicQueue() {}

private:
    T m_Data[Capacity];
    std::atomic<size_t> m_Size;
    std::atomic<size_t> m_Head;
    std::atomic<size_t> m_Tail;
};

#endif
