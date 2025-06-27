#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <functional>
#include <memory>
#include <thread>

#include "mpmc_queue.hpp"

namespace spm
{

template <typename In, typename Out>
class node
{
public:
    node(std::function<Out(In)>&& func) : m_func(std::move(func))
    {
        m_in = std::make_shared<mpmc_queue<In>>();
        m_out = std::make_shared<mpmc_queue<Out>>();

        m_worker = new std::thread([this]() {
            std::optional<In> input;
            while (true)
            {
                input = m_in->pop();
                if (!input.has_value())
                {
                    m_out->close();
                    return;
                }

                m_out->push(m_func(input.value()));
            }
        });
    }

    void send(In&& input) { m_in->push(std::move(input)); }

    void send(const In& input) { m_in->push(input); }

    Out recv() { return m_out->pop().value(); }

    template <typename Out2>
    void connect_to(const node<Out, Out2>& other)
    {
        m_out = other.m_in;
    }

    ~node()
    {
        m_in->close();
        m_worker->join();

        delete m_worker;
    }

private:
    std::thread* m_worker;
    std::shared_ptr<mpmc_queue<In>> m_in;
    std::shared_ptr<mpmc_queue<Out>> m_out;
    std::function<Out(In)> m_func;
};

} // namespace spm

#endif
