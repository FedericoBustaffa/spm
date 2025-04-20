#include <cassert>
#include <random>

#include "matrix.hpp"
#include "timer.hpp"

void init(Matrix& m)
{
    std::mt19937 engine(42);
    std::uniform_real_distribution<float> dist(50.0f, 100.0f);
    for (size_t i = 0; i < m.rows(); i++)
        for (size_t j = 0; j < m.cols(); j++)
            m(i, j) = dist(engine);
}

Matrix naive_mm(const Matrix& a, const Matrix& b)
{
    assert(a.cols() == b.rows());
    Matrix res(a.rows(), b.cols());
    for (size_t i = 0; i < a.rows(); i++)
        for (size_t j = 0; j < b.cols(); j++)
            for (size_t k = 0; k < a.cols(); k++)
                res(i, j) += a(i, k) * b(k, j);

    return res;
}

Matrix looporder_mm(const Matrix& a, const Matrix& b)
{
    assert(a.cols() == b.rows());
    Matrix res(a.rows(), b.cols());
    for (size_t i = 0; i < a.rows(); i++)
        for (size_t k = 0; k < a.cols(); k++)
            for (size_t j = 0; j < b.cols(); j++)
                res(i, j) += a(i, k) * b(k, j);

    return res;
}

Matrix transpose_mm(const Matrix& a, const Matrix& b)
{
    assert(a.cols() == b.rows());
    Matrix res(a.rows(), b.cols());
    Matrix bt = b.transpose();
    for (size_t i = 0; i < a.rows(); i++)
    {
        for (size_t j = 0; j < bt.rows(); j++)
        {
            for (size_t k = 0; k < a.cols(); k++)
                res(i, j) += a(i, k) * bt(j, k);
        }
    }

    return res;
}

Matrix tiled_mm(const Matrix& a, const Matrix& b, size_t T)
{
    assert(a.cols() == b.rows());
    Matrix res(a.rows(), b.cols());

    for (size_t i = 0; i < a.rows(); i += T)
        for (size_t j = 0; j < b.cols(); j += T)
            for (size_t k = 0; k < a.cols(); k += T)
                for (size_t ii = i; ii < std::min(i + T, a.rows()); ii++)
                    for (size_t jj = j; jj < std::min(j + T, b.cols()); jj++)
                        for (size_t kk = k; kk < std::min(k + T, a.cols());
                             kk++)
                            res(ii, jj) += a(ii, kk) * b(kk, jj);

    return res;
}

Matrix tiled_loop_mm(const Matrix& a, const Matrix& b, size_t T)
{
    assert(a.cols() == b.rows());
    Matrix res(a.rows(), b.cols());

    for (size_t i = 0; i < a.rows(); i += T)
        for (size_t k = 0; k < a.cols(); k += T)
            for (size_t j = 0; j < b.cols(); j += T)
                for (size_t ii = i; ii < std::min(i + T, a.rows()); ii++)
                    for (size_t kk = k; kk < std::min(k + T, a.cols()); kk++)
                        for (size_t jj = j; jj < std::min(j + T, b.cols());
                             jj++)
                            res(ii, jj) += a(ii, kk) * b(kk, jj);

    return res;
}

int main(int argc, const char** argv)
{
    size_t n = 128, m = 128, l = 128;
    if (argc == 2)
    {
        n = std::atoi(argv[1]);
        m = std::atoi(argv[1]);
        l = std::atoi(argv[1]);
    }
    else if (argc == 4)
    {
        n = std::atoi(argv[1]);
        m = std::atoi(argv[2]);
        l = std::atoi(argv[3]);
    }

    Matrix a(n, m);
    Matrix b(m, l);

    init(a);
    init(b);

    // results files
    Timer timer;
    timer.start();
    Matrix res = naive_mm(a, b);
    double naive_time = timer.stop();
    std::printf("%s elapsed time: %g ns\n", "naive", naive_time);

    timer.start();
    res = transpose_mm(a, b);
    double transpose_time = timer.stop();
    std::printf("%s elapsed time: %g ns - ", "transpose", transpose_time);
    std::printf("transpose speed up: %.2g\n", naive_time / transpose_time);

    timer.start();
    res = looporder_mm(a, b);
    double loop_time = timer.stop();
    std::printf("%s elapsed time: %g ns - ", "looporder", loop_time);
    std::printf("looporder speed up: %.2g\n", naive_time / loop_time);

    const size_t T = 64;
    timer.start();
    res = tiled_mm(a, b, T);
    double tiled_time = timer.stop();
    std::printf("%s elapsed time: %g ns - ", "tile", tiled_time);
    std::printf("tile size %lu speed up: %.2g\n", T, naive_time / tiled_time);

    timer.start();
    res = tiled_loop_mm(a, b, T);
    double tiled_loop_time = timer.stop();
    std::printf("%s elapsed time: %g ns - ", "tile_loop", tiled_loop_time);
    std::printf("tile size %lu loop speed up: %.2g\n", T,
                naive_time / tiled_loop_time);

    return 0;
}
