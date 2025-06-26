#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <functional>
#include <thread>

#include "spsc_queue.hpp"

namespace spm
{

template <typename In, typename Out>
class node
{
public:
    node(std::function<Out(In)>&& func) : m_func(std::move(func))
    {
        m_worker = new std::thread([this]() {
            std::optional<In> input;
            while (true)
            {
                input = m_in.pop();
                if (!input.has_value())
                    break;

                m_out.push(m_func(input.value()));
            }
        });
    }

    Out operator()(const In& input)
    {
        m_in.push(input);
        return m_out.pop().value();
    }
    Out operator()(In&& input)
    {
        m_in.push(std::move(input));
        return m_out.pop().value();
    }

    ~node()
    {
        m_in.close();
        m_out.close();
        m_worker->join();
        delete m_worker;
    }

private:
    std::thread* m_worker;
    spsc_queue<In> m_in;
    spsc_queue<Out> m_out;
    std::function<Out(In)> m_func;
};

} // namespace spm

#endif
