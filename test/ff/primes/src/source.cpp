#include "components.hpp"

Source::Source(uint64_t n) : n(n) {}

uint64_t* Source::svc(uint64_t*)
{
    for (uint64_t i = 2; i < n; i++)
        ff_send_out(new uint64_t(i));

    return EOS;
}
