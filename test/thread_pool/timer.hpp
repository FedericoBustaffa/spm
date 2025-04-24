#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer
{
public:
    Timer() = default;

    void start() { m_Start = std::chrono::high_resolution_clock::now(); }

    double stop()
    {
        m_End = std::chrono::high_resolution_clock::now();
        return (m_End - m_Start).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start, m_End;
};

#endif
