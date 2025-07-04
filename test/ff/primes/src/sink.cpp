#include "components.hpp"

uint64_t* Sink::svc(uint64_t* n)
{
    m_counter += *n;
    delete n;

    return GO_ON;
}
