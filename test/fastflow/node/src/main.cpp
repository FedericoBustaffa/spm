#include <cstdio>

#include <ff/node.hpp>

struct node : public ff::ff_node_t<int, int>
{
    int* svc(int* in)
    {
        std::printf("%d\n", *in);
        return this->GO_ON;
    }

    int run_and_wait_end(bool = false)
    {
        if (this->run() < 0)
            return -1;

        return this->wait();
    }
};

int main(int argc, const char** argv)
{
    node n;
    n.run_and_wait_end();

    return 0;
}
