#include <iostream>
#include <random>
#include <vector>

#include <omp.h>

using task_t = std::pair<float, size_t>;
const size_t minVsize = 512;
const size_t maxVsize = 8192;

inline float dotprod(std::vector<float>& V1, std::vector<float>& V2)
{
    float sum = 0.0;
    for (size_t i = 0; i < V1.size(); ++i)
        sum += V1[i] * V2[i];

    return sum;
}

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cerr << "use: " << argv[0] << " stream-length [nworkers]\n";
        return -1;
    }
    long length = std::stol(argv[1]);
    if (length <= 0)
    {
        std::cerr << "stream-length should be greater than 0\n";
        return -1;
    }

    ssize_t nworkers = omp_get_max_threads();
    if (argc == 3)
    {
        nworkers = std::stol(argv[2]);
        if (nworkers <= 0)
        {
            std::cerr << "nworkers should be greater than 0\n";
            return -1;
        }
    }

    auto random01 = []() {
        static std::mt19937 generator;
        std::uniform_real_distribution<float> distribution(0, 1);
        return distribution(generator);
    };
    auto random = [](const int& min, const int& max) {
        static std::mt19937 generator;
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    };

    float sum{0.0};

    auto start = omp_get_wtime();

#pragma omp parallel num_threads(nworkers)
    {

#pragma omp single
        for (long i = 0; i < length; ++i)
        {

            float x = random01();
            long size = random(minVsize, maxVsize);

#pragma omp task default(none) firstprivate(size, x) shared(sum)
            {
                std::vector<float> V1(size);
                std::vector<float> V2(size);

                for (long i = 0; i < size; ++i)
                {
                    V1[i] = x * i;
                    V2[i] = x * i / size;
                }
                float r = dotprod(V1, V2);

#pragma omp atomic
                sum += r;
            }
        }
    }
    auto stop = omp_get_wtime();

    std::printf("sum= %.4f\n", std::sqrt(sum));
    std::printf("Time= %f (ms)\n", (stop - start) * 1000.0);

    return 0;
}
