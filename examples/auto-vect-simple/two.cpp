#include <iostream>
#include <vector>

#include "utimer.hpp"

int f(int x) { return (x % 2); }

int main()
{
    int n = 2048;
    std::vector<int> x(n);

    {
        utimer t("all");
        for (int i = 0; i < n; i++)
            x[i] = f(i);

        for (int i = 1; i < n; i++)
            x[0] += x[i];
    }
    std::cout << x[0] << std::endl;

    return 0;
}
