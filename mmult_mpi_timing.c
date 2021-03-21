/** 
 * 
 * Program to multiply a matrix times a vector using both
 * MPI to distribute the computation among nodes and OMP
 * 
 * Compile: mpigcc -g -Wall mmult_mpi mmult_mpi.c
 * Run: mpiexec -f ~/hosts -n 12 ./mmult_mpi 500
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mat.h"
#define min(x, y) ((x)<(y)?(x):(y))

void compute_inner_product(double *buffer, int bCols, MPI_Datatype datatype, int source, int tag,
             MPI_Comm mpi_comm, MPI_Status status, int process_id, int bRows, double *b, int row, 
             int ans); 

void master_code(double *aa, double *b, double *c, double *buffer, double ans, int nrows, int ncols, int master, int numprocs,
                  MPI_Status status);

double mmult_mpi(int argc, char* argv[])
{
    int nrows, ncols;
    double *aa, *b, *c;
    double *buffer, ans;
    double *times;
    double total_times;
    int run_index;
    int nruns;
    int myid, master, numprocs;
    double starttime, endtime;
    MPI_Status status;
    int i, j, numsent, sender;
    int anstype, row;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (argc > 1) {
        nrows = atoi(argv[1]);
        ncols = nrows;
        // aa = (double*)malloc(sizeof(double) * nrows * ncols);
        b = (double*)malloc(sizeof(double) * ncols);
        c = (double*)malloc(sizeof(double) * nrows);
        buffer = (double*)malloc(sizeof(double) * ncols);
        master = 0;
        if (myid == master) {
            master_code(aa, b, c, buffer, ans, nrows, ncols, master, numprocs, status);
        } else {
            // Slave Code goes here
            MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            compute_inner_product(buffer, ncols, MPI_DOUBLE, master, 
                                  MPI_ANY_TAG, MPI_COMM_WORLD, status,
                                  myid, nrows, b, row, ans);
        }
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
    return endtime - starttime;
}

FILE * open_output_file() {
    FILE *cfPtr;
    if((cfPtr = fopen("output/mpi_output.txt", "w")) == NULL){
    puts("File cannot be opened");
    }
    return cfPtr;  
}

int main(int argc, char **argv) {

    // if(argc == 3) {
    //     // matrices a and b provided
    //     int n = get_matrix_size_from_file(argv[1]);
    //     a = read_matrix_from_file(argv[1]);
    //     b = read_matrix_from_file(argv[2]);
    // }
    // else if(argc == 1) {
    //     // business as usual, gen a random square matrices of size argv[1]
    // }
    // else {
    //     // run through the matrices stored in the input dir
    // }
    // // no matrices provided (pass a default value)


    double delta_t = mmult_mpi(argc, argv);
    // f_ptr = open_file();
    return 0;
}


void master_code(double *aa, double *b, double *c, double *buffer, double ans, int nrows, int ncols, int master, int numprocs,
                  MPI_Status status) {
                    
    double starttime, endtime;
    int anstype, numsent, sender;

    // Master Code goes here
    aa = gen_matrix(nrows, ncols);
    starttime = MPI_Wtime();
    numsent = 0;
    MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
    for (int i = 0; i < min(numprocs-1, nrows); i++) {
        for (int j = 0; j < ncols; j++) {
            buffer[j] = aa[i * ncols + j];
        }  
        MPI_Send(buffer, ncols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
        numsent++;
    }
    for (int i = 0; i < nrows; i++) {
        MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;
        anstype = status.MPI_TAG;
        c[anstype-1] = ans;
        if (numsent < nrows) {
            for (int j = 0; j < ncols; j++) {
                buffer[j] = aa[numsent*ncols + j];
            }  
            MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD);
            numsent++;
        } else {
            MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
        }
    } 
    endtime = MPI_Wtime();
    printf("%f\n",(endtime - starttime));
}


void compute_inner_product(double *buffer, int bCols, MPI_Datatype datatype, int source, int tag,
             MPI_Comm mpi_comm, MPI_Status status, int process_id, int bRows, double *b, int row, 
             int ans) 
{

    // each slave process id corresponds to the ith row it will be responsible for
    if (process_id <= bRows) {

        while(1) {

            MPI_Recv(buffer, bCols, datatype, source, tag, mpi_comm, &status);
            if (status.MPI_TAG == 0) {
                break;
            }
            row = status.MPI_TAG;
            ans = 0.0;
            for (int j = 0; j < bCols; j++) {
                ans += buffer[j] * b[j];
            }
            // send answer to master, along with the row #
            MPI_Send(&ans, 1, datatype, source, row, mpi_comm);
        }
    }
}
