#ifndef SPIN_COUNTER_HPP
#define SPIN_COUNTER_HPP

#include <cstdint>

#include "spin_lock.hpp"

class spin_counter
{
public:
    spin_counter() : m_counter(0) {}

    void increment()
    {
        m_mutex.lock();
        m_counter++;
        m_mutex.unlock();
    }

    int64_t get() const { return m_counter; }

    ~spin_counter() {}

private:
    int64_t m_counter;
    spin_lock m_mutex;
};

#endif
