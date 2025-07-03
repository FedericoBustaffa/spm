#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <cstdint>

#include <ff/ff.hpp>

using namespace ff;

class Source : public ff_node_t<uint64_t>
{
public:
    Source(uint64_t n);

    uint64_t* svc(uint64_t*) override;

private:
    uint64_t n;
};

class Worker : public ff_node_t<uint64_t>
{
public:
    uint64_t* svc(uint64_t* n) override;
};

class Sink : public ff_minode_t<uint64_t>
{
public:
    uint64_t* svc(uint64_t* n) override;

    inline uint64_t counter() const { return m_counter; }

private:
    uint64_t m_counter = 0;
};

#endif
