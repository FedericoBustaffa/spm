#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace spm
{

class timer
{

public:
    timer() = default;

    inline void start() { m_start = std::chrono::system_clock::now(); }

    inline double stop()
    {
        std::chrono::duration<double> elapsed =
            std::chrono::system_clock::now() - m_start;

        return elapsed.count();
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_start;
};

} // namespace spm

#endif
