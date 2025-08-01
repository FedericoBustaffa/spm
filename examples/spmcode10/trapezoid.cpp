#include <cstdio>
#include <mpi.h>

// Function to be integrated
double f(double x) { return 4.0 / (1.0 + x * x); }

// Compute the area of function f(x) for x=[a, b].
// The interval is partitioned into n subintervals of equal size.
double trapezoid(int myrank, int size, double a, double b, int n)
{
    const double h = (b - a) / n;
    const int local_start = n * myrank / size;
    const int local_end = n * (myrank + 1) / size;
    double x = a + local_start * h;
    double area = 0.0;

    for (int i = local_start; i < local_end; ++i)
    {
        area += h * (f(x) + f(x + h)) / 2.0;
        x += h;
    }
    return area;
}

int main(int argc, char* argv[])
{
    const double a = 0.0;
    const double b = 1.0;
    const int n = 10000000;
    double partial_result;
    double result = 0.0;
    int myrank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // each node computes a partial result
    partial_result = trapezoid(myrank, size, a, b, n);

    if (myrank)
    { // all but the master (rank 0) send their partial result
        MPI_Send(&partial_result, 1, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD);
    }
    else
    { // master (rank 0)
        // it collects all partial results from other processes
        result = partial_result;
        for (int p = 1; p < size; ++p)
        {
            MPI_Recv(
                &partial_result, 1, MPI_DOUBLE,
                p, // <-- potential deadlock (Why?), better use MPI_ANY_SOURCE
                100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result += partial_result;
        }
        std::printf("Area: %.18f\n", result);
    }

    MPI_Finalize();
    return 0;
}
