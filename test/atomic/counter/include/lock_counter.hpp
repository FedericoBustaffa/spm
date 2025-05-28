#ifndef LOCK_COUNTER_HPP
#define LOCK_COUNTER_HPP

#include <cstdint>
#include <mutex>

class lock_counter
{
public:
    lock_counter() : m_counter(0) {}

    void increment()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_counter++;
    }

    int64_t get()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_counter;
    }

private:
    int64_t m_counter;
    std::mutex m_mutex;
};

#endif
