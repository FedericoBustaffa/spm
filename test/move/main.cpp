#include <iostream>

#include "buffer.hpp"

int main(int argc, const char **argv)
{
    buffer a(10); // default constructor
    buffer b(a);  // std copy constructor

    buffer c = b; // this will trigger copy construct
    c = a;        // this will trigger assignment copy operator

    buffer d(std::move(c)); // move constructor
    d = std::move(a);       // move assign operator

    return 0;
}
