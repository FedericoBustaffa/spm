#include <iostream>
#include <vector>

#include "benchmarks.hpp"
#include "timer.hpp"

int main(int argc, const char** argv)
{
    int64_t e = 20;

    if (argc >= 2)
        e = std::atol(argv[1]);

    int64_t n = 1 << e;
    std::cout << n << std::endl;

    std::vector<int> numbers = generate_numbers(n);

    Timer timer;
    timer.start();
    sequential(numbers);
    double stime = timer.stop();
    std::cout << "sequential time: " << stime << " seconds" << std::endl;

    // timer.start();
    // block(numbers);
    // double block_time = timer.stop();
    // std::cout << "block time: " << block_time << " seconds" << std::endl;
    //
    // timer.start();
    // cyclic(numbers);
    // double cyclic_time = timer.stop();
    // std::cout << "cyclic time: " << cyclic_time << " seconds" << std::endl;
    //
    // timer.start();
    // cyclic(numbers, 16);
    // double block_cyclic_time = timer.stop();
    // std::cout << "block cyclic time: " << block_cyclic_time << " seconds"
    //           << std::endl;

    timer.start();
    dynamic(numbers);
    double dynamic_time = timer.stop();
    std::cout << "dynamic time: " << dynamic_time << std::endl;

    return 0;
}
