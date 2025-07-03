#include "components.hpp"

Worker::Worker(const std::function<uint64_t(uint64_t)>& func) : m_func(func) {}

uint64_t* Worker::svc(uint64_t* n)
{
    uint64_t* value = new uint64_t(m_func(*n));
    ff_send_out(value);
    delete n;

    return GO_ON;
}
