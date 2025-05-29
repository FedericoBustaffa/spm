#include <cstdio>

#include "spsc_queue.hpp"

int main(int argc, char** argv)
{
    std::printf("Hello World\n");
    spsc_queue<int> queue(10);

    return 0;
}
