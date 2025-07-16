#include <cstdio>
#include <mpi.h>

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::printf("communicator size: %d\n", size);

    char name[128];
    int len;
    MPI_Get_processor_name(name, &len);
    std::printf("name: %s\n", name);

    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    std::printf("ID: %d\n", id);

    MPI_Finalize();

    return 0;
}
