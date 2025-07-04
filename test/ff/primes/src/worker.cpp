#include "components.hpp"

uint64_t* Worker::svc(uint64_t* n)
{
    if (*n < 2)
    {
        delete n;
        return GO_ON;
    }

    for (uint64_t i = 2; i * i <= *n; i++)
    {
        if (*n % i == 0)
        {
            delete n;
            return GO_ON;
        }
    }

    delete n;
    ff_send_out(new uint64_t(1));

    return GO_ON;
}
