#include <cstdio>
#include <cstring>
#include <mpi.h>

int main(int argc, char* argv[])
{
    char message[20];
    int myrank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if (myrank == 0) /* code for process zero */
    {
        std::strcpy(message, "Hello, there");
        size_t len = std::strlen(message) + 1;
        MPI_Send(message, len, MPI_CHAR, 1, 99, MPI_COMM_WORLD);
    }
    else if (myrank == 1) /* code for process one */
    {
        MPI_Recv(message, 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_CHAR, &count);
        std::printf("received: %s (MPI_CHAR received %d)\n", message, count);
    }
    MPI_Finalize();

    return 0;
}
