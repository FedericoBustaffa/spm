#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class timer
{
public:
    void start()
    {
        start_tp = std::chrono::high_resolution_clock::now();
    }

    double stop()
    {
        stop_tp = std::chrono::high_resolution_clock::now();
        duration = stop_tp - start_tp;

        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_tp, stop_tp;
    std::chrono::duration<double> duration;
};

#endif
