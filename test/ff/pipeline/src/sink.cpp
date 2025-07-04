#include "components.hpp"

uint64_t* Sink::svc(uint64_t* n)
{
    m_result += *n;
    delete n;

    return GO_ON;
}
