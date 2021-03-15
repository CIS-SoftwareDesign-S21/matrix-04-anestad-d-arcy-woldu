/**
 * mpi_hello.c
 *
 * Greets the world, MPI-style
 *
 * Compile:  mpicc -g -Wall -o mpi_hello mpi_hello.c
 * Run:      mpirun -n <num_procs> --hostfile host_file.txt ./mpi_hello
 *           or just: mpirun -n 2 ./mpi_hello
 */

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    /* Total number of processes in this MPI communicator */
    int comm_sz;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Get the rank of this process */
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Hostname */
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int name_sz;
    MPI_Get_processor_name(hostname, &name_sz);

    printf("Hello from %s, rank %d (of %d).\n", hostname, rank, comm_sz);

    /* Clean up */
    MPI_Finalize();
    return 0;
}

