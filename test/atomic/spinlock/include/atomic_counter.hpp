
#ifndef ATOMIC_COUNTER_HPP
#define ATOMIC_COUNTER_HPP

#include <atomic>
#include <cstdint>

class atomic_counter
{
public:
    atomic_counter() : m_counter(0) {}

    void increment() { m_counter.fetch_add(1); }

    int64_t get() { return m_counter.load(); }

private:
    std::atomic<int64_t> m_counter;
};

#endif
