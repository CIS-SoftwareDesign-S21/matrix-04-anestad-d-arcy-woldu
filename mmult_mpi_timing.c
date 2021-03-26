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
                  MPI_Status status, FILE *output_ptr);

void mmult_mpi(int argc, char* argv[], double *aa, double *b, int nrows, FILE *output_ptr);

void loop_mmult_mpi(int argc, char* argv[]);


FILE * open_file(const char * path, const char mode) {
    /*
        This code 
    */
    FILE *cfPtr;
    if((cfPtr = fopen(path, mode)) == NULL) {
        puts("File cannot be opened");
    }
    return cfPtr;  
}

int main(int argc, char **argv) {   

    double delta_t;
    double *a, *b;
    FILE *output_ptr;
    int n, m;

    output_ptr = open_file("output/sample_log.txt", 'a');

    if(argc == 2) {
        // business as usual, gen a random square matrices of size argv[1]
        output_ptr = open_file("mpi_sample_log.txt", 'a');
        n = atoi(argv[1]);
        m = atoi(argv[1]);

        a = gen_matrix(m, n);
        b = gen_matrix(m, n);
        mmult_mpi(argc, argv, a, b, n,output_ptr);

    }
    else if(argc == 3) {
        // matrices a and b are provided for
        output_ptr = open_file("mpi_multirun_log.txt", 'a');
        
        n = get_matrix_size_from_file(argv[1]);
        a = read_matrix_from_file(argv[1]);
        b = read_matrix_from_file(argv[2]);
        mmult_mpi(argc, argv, a, b, n, output_ptr);
        sleep(2);
    }
    return 0;
}


void mmult_mpi(int argc, char* argv[], double *aa, double *b, int nrows, FILE *output_ptr) {
    int ncols;
    double *c;
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
        ncols = nrows;

        c = (double*)malloc(sizeof(double) * nrows);
        buffer = (double*)malloc(sizeof(double) * ncols);
        master = 0;

        if (myid == master) {
            master_code(aa, b, c, buffer, ans, nrows, ncols, master, numprocs, status, output_ptr);
        } else {
            MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            // Slave process receives matrix b sent from master and computes inner product
            compute_inner_product(buffer, ncols, MPI_DOUBLE, master, 
                                    MPI_ANY_TAG, MPI_COMM_WORLD, status,
                                    myid, nrows, b, row, ans);
        }
    free(c);
    free(buffer);
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
}


void master_code(double *aa, double *b, double *c, double *buffer, double ans, int nrows, int ncols, int master, int numprocs,
                  MPI_Status status, FILE *output_ptr) {
                    
    double starttime, endtime;
    int anstype, numsent, sender;

    starttime = MPI_Wtime();
    numsent = 0;
    MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);

    // iterate through matrix b and send its columns to each slave process
    for (int i = 0; i < min(numprocs-1, nrows); i++) {
        for (int j = 0; j < ncols; j++) {
            buffer[j] = aa[i * ncols + j];
        }  
        MPI_Send(buffer, ncols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
        numsent++;
    }
    for (int i = 0; i < nrows; i++) {
        // Recieve the answers computed by the slave processes and append it to matrix c
        MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;
        anstype = status.MPI_TAG;
        c[anstype-1] = ans;

        // if we haven't finished the job, keep sending more rows to the slave processes, until all rows of matrix aa are computed for
        if (numsent < nrows) {
            for (int j = 0; j < ncols; j++) {
                buffer[j] = aa[numsent*ncols + j];
            }  
            MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD);
            numsent++;
        } else {
            // we're finished, job done!
            MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
        }
    } 
    endtime = MPI_Wtime();
    fprintf(output_ptr, "%d", nrows);
    fprintf(output_ptr, ", %f\n", (endtime - starttime));
    fclose(output_ptr);
    printf("%f\n",(endtime - starttime));
}


void compute_inner_product(double *buffer, int bCols, MPI_Datatype datatype, int source, int tag,
             MPI_Comm mpi_comm, MPI_Status status, int process_id, int bRows, double *b, int row, 
             int ans) {

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
