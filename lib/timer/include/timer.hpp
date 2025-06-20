#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace spm
{

class timer
{
public:
    timer() = default;

    void start() { m_start = std::chrono::high_resolution_clock::now(); }

    double stop()
    {
        m_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = m_end - m_start;

        return elapsed.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_end;
};

} // namespace spm

#endif
