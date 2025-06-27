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
        m_in = new spsc_queue<In>();
        m_out = new spsc_queue<Out>();

        m_worker = new std::thread([this]() {
            std::optional<In> input;
            while (true)
            {
                input = m_in->pop();
                if (!input.has_value())
                    return;

                m_out->push(m_func(input.value()));
            }
        });
    }

    void send(In&& input) { m_in->push(std::move(input)); }

    void send(const In& input) { m_in->push(input); }

    Out recv() { return m_out->pop().value(); }

    template <typename Out2>
    void connect_to(node<Out, Out2>& other)
    {
        delete m_out;
        m_out = other.m_in;
    }

    Out operator()(const In& input)
    {
        m_in->push(input);
        return m_out->pop().value();
    }

    Out operator()(In&& input)
    {
        m_in->push(std::move(input));
        return m_out->pop().value();
    }

    ~node()
    {
        m_in->close();
        m_worker->join();

        delete m_worker;
        delete m_in;
        delete m_out;
    }

private:
    std::thread* m_worker;
    spsc_queue<In>* m_in;
    spsc_queue<Out>* m_out;
    std::function<Out(In)> m_func;
};

} // namespace spm

#endif
