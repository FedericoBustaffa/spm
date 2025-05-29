#ifndef SPIN_LOCK_HPP
#define SPIN_LOCK_HPP

#include <atomic>

class spin_lock
{
public:
    spin_lock() : acquired(ATOMIC_FLAG_INIT) {}

    void lock()
    {
        while (acquired.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock() { acquired.clear(std::memory_order_release); }

    ~spin_lock() {}

private:
    std::atomic_flag acquired;
};

#endif
