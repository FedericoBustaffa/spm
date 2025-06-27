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
    node(std::function<Out(In)>&& func)
        : m_in(std::make_shared<mpmc_queue<In>>()),
          m_out(std::make_shared<mpmc_queue<Out>>()), m_func(std::move(func)),
          m_worker([this]() {
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
          })
    {
    }

    virtual void send(In&& input) { m_in->push(std::move(input)); }

    virtual void send(const In& input) { m_in->push(input); }

    virtual Out recv() { return m_out->pop().value(); }

    virtual void connect_to(const node& other)
    {
        m_out.emplace_back(other.m_in);
    }

    virtual ~node() { m_worker.join(); }

protected:
    std::vector<mpmc_queue<In>&> m_in;
    std::vector<mpmc_queue<Out>&> m_out;
    std::function<Out(In)> m_func;
    std::thread m_worker;
};

template <typename In, typename Out>
class source_node : public node<In, Out>
{
public:
    source_node(std::function<Out(In)>&& func) : node<In, Out>(std::move(func))
    {
    }

    ~source_node() { this->m_in->close(); }
};

template <typename In, typename Out>
class sink_node : public node<In, Out>
{
public:
    sink_node(std::function<Out(In)>&& func) : node<In, Out>(std::move(func)) {}

    ~sink_node() {}
};

} // namespace spm

#endif
