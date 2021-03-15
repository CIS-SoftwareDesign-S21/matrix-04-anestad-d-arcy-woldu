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

const int MAX_STRING = 100;

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);
    char  greeting[MAX_STRING];

    /* Total number of processes in this MPI communicator */
    int comm_sz;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Get the rank of this process */
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Hostname */
   // char hostname[MPI_MAX_PROCESSOR_NAME];
   // int name_sz;
   // MPI_Get_processor_name(hostname, &name_sz);

   // printf("Hello from %s, rank %d (of %d).\n", hostname, rank, comm_sz);


   if (rank != 0) {
 
      /* Create message */
      sprintf(greeting, "Greetings from process %d of %d!", rank, comm_sz);

      /* Send message to process 0 */
      MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
 
   } else { 

      int q;
      /* Print my message */
      printf("Greetings from process %d of %d!\n", rank, comm_sz);
      for (q = 1; q < comm_sz; q++) {
         /* Receive message from process q */
         MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         /* Print message from process q */
         printf("%s\n", greeting);
      } 
   }




    /* Clean up */
    MPI_Finalize();
    return 0;
}

