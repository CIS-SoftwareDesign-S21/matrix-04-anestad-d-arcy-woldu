/** 
  * 
  * Program to multiply a matrix times a vector using both
  * MPI to distribute the computation among nodes and OMP
  * 
  * Compile: mpigcc -g -Wall mmult_mpi mmult_mpi.c
  * Run: mpiexec -f ~/hosts -n 3 ./mmult_mpi 500
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mat.h"
#define min(x, y) ((x)<(y)?(x):(y))


void compute_inner_product(double *buffer, int bCols, MPI_Datatype datatype, int source, int tag,
             MPI_Comm comm, MPI_Status status, int process_id, int bRows, double *b, int row, 
             int ans) {

    // each slave process id corresponds to the ith row it will be responsible for
    if (process_id <= bRows) {

        while(1) {

            MPI_Recv(buffer, bCols, datatype, source, tag, comm, &status);
            if (status.MPI_TAG == 0) {
                break;
            }
            row = status.MPI_TAG;
            ans = 0.0;
            for (int j = 0; j < bCols; j++) {
                ans += buffer[j] * b[j];
            }
            // send answer to master, along with the row #
            MPI_Send(&ans, 1, datatype, source, row, comm);
        }
    }
}

int mmult_mpi(int argc, char **argv) {
    double *aa, *b, *c;
    int aRows, aCols;
    int bRows, bCols;
    double *buffer, ans;
    double *times;
    double total_times;
    int run_index;
    int nruns;
    int myid, master, numprocs;
    double starttime, endtime;
    MPI_Status status;
    int i, j, numsent, sender;
    int nrows, ncols;
    int anstype, row;
    MPI_Init(argc, &argv);
    srand(time(0));
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (argc > 1) {

        nrows = atoi(argv[1]);
        ncols = nrows;
        bCols = ncols;
        bRows = nrows;
        // aa = (double*)malloc(sizeof(double) * nrows * ncols);
        b = (double*)malloc(sizeof(double) * ncols);
        c = (double*)malloc(sizeof(double) * nrows);

        buffer = (double*)malloc(sizeof(double) * bCols);
        master = 0;

        // Master Process starts here 
        if (myid == master) {
            printf("Master Process running!\n");
            // load matrix a and b here
            aa = gen_matrix(nrows, ncols);

            starttime = MPI_Wtime();
            numsent = 0;
            MPI_Bcast(b, bCols, MPI_DOUBLE, master, MPI_COMM_WORLD);

            // iterate through matrix b and send its columns to each slave process
            for (i = 0; i < min(numprocs-1, bRows); i++) {
                for (j = 0; j < bCols; j++) {
                        buffer[j] = aa[i * bCols + j];
                    }  
                MPI_Send(buffer, bCols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
                numsent++;
            }
                // Recieve the answers computed by the slave processes and append it to matrix c
            for (i = 0; i < bRows; i++) {
                MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                sender = status.MPI_SOURCE;
                anstype = status.MPI_TAG;
                c[anstype-1] = ans;
                // if we haven't finished the job, keep sending more rows to the slave processes, until all rows are accounted for
                if (numsent < bRows) {
                    for (j = 0; j < bCols; j++) {
                        buffer[j] = aa[numsent*bCols + j];
                    }  
                    MPI_Send(buffer, bCols, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD);
                    numsent++;
                }  else {
                        // we're finished, job done!
                        MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
                }
            } 
            endtime = MPI_Wtime();
            printf("%f\n",(endtime - starttime));
        } else {
            // Slave process receives matrix b sent from master and computes inner product
            printf("Slave Process running!\n");
            compute_inner_product(&buffer, bCols, MPI_DOUBLE, master, MPI_ANY_TAG, MPI_COMM_WORLD, status,
                                    myid, bRows, b, row, ans);

            MPI_Bcast(b, bCols, MPI_DOUBLE, master, MPI_COMM_WORLD);
        }
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
    return 0;
}

int main(int argc, char *argv[]) {
    int to_return = mmult_mpi(argc, argv);
    return to_return;
}
