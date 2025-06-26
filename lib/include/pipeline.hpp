#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <thread>
#include <vector>

#include "mpmc_queue.hpp"
#include "stage.hpp"

namespace spm
{

template <typename In, typename Out>
class pipeline
{
public:
    pipeline() {}

    template <typename... Args>
    void add_stage(stage<Out, Args...>&& stage)
    {
        auto work = [&](mpmc_queue<In>&& in, mpmc_queue<Out>&& out) {
            std::optional<In> input;
            while (true)
            {
                input = in.pop();
                if (!input.has_value())
                    break;

                stage(input);
            }
        };

        m_stages.emplace_back(work, mpmc_queue<In>(), mpmc_queue<Out>(), stage);
    }

    template <typename T>
    T* run(T* input)
    {
        return nullptr;
    }

    ~pipeline() {}

private:
    std::vector<std::thread> m_stages;
};

} // namespace spm

#endif
