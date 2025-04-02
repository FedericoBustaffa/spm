#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <iostream>

class timer
{
public:
    timer()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    ~timer()
    {
    }

    void reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    double lap()
    {
        m_Stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = m_Stop - m_Start;

        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start, m_Stop;
};

#endif
