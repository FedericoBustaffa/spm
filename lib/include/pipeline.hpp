#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <functional>
#include <thread>
#include <vector>

#include "mpmc_queue.hpp"

namespace spm
{

class pipeline
{
public:
    pipeline() {}

    template <typename Func>
    void add_stage(Func&& func)
    {
        auto work = []() {};

        m_stages.emplace_back(work, m_queues.front(), func);
    }

    template <typename T>
    T* run(T* input)
    {
        return nullptr;
    }

    ~pipeline() {}

private:
    std::vector<std::thread> m_stages;
    std::vector<mpmc_queue<...>> m_queues;
};

} // namespace spm

#endif
