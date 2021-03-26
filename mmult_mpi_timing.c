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

void mmult_mpi(int argc, char* argv[], double *aa, double *b, int nrows, FILE *output_ptr, int test_output);

void loop_mmult_mpi(int argc, char* argv[]);

FILE * open_output_file(const char * path) {
    FILE *cfPtr;
    if((cfPtr = fopen(path, "a")) == NULL) {
        puts("File cannot be opened");
    }
    return cfPtr;  
}

int main(int argc, char **argv) {   
    int test_output;
    double *a, *b;
    FILE *output_ptr;
    int n, m;

    if(argc == 2) {
        // business as usual, gen a random square matrices of size argv[1]

        output_ptr = open_output_file("output/mpi_output.txt");
        test_output = 0;

        n = atoi(argv[1]);
        m = atoi(argv[1]);

        a = gen_matrix(m, n);
        b = gen_matrix(m, n);
        mmult_mpi(argc, argv, a, b, n, output_ptr, test_output);

    }
    else if(argc == 3) {
        // matrices a and b provided
        output_ptr = open_output_file("output/mpi_output.txt");
        test_output = 0;

        n = get_matrix_size_from_file(argv[1]);
        a = read_matrix_from_file(argv[1]);
        b = read_matrix_from_file(argv[2]);
        mmult_mpi(argc, argv, a, b, n, output_ptr, test_output);
        sleep(1);
    }
    else if(argc == 4) {
        // run tests

        test_output = 1;
        output_ptr = open_output_file("output/mpi_output.txt");
    }
    return 0;
}

void mmult_mpi(int argc, char* argv[], double *aa, double *b, int nrows, FILE *output_ptr, int test_output) {
    int ncols;
    double *c;
    double *buffer, ans;
    int myid, master, numprocs;
    MPI_Status status;
    int row;

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
            // master code starts here, which delegates tasks to compute
            master_code(aa, b, c, buffer, ans, nrows, ncols, master, numprocs, status, output_ptr);
        } else {

            // Slave process receives matrix b sent from master and computes inner product
            MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            compute_inner_product(buffer, ncols, MPI_DOUBLE, master, 
                                    MPI_ANY_TAG, MPI_COMM_WORLD, status,
                                    myid, nrows, b, row, ans);
        }
    
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

    // send matrix b to all the slave processes
    MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);

    // iterate through matrix a and send its columns to each slave processes
    for (int i = 0; i < min(numprocs-1, nrows); i++) {
        for (int j = 0; j < ncols; j++) {
            buffer[j] = aa[i * ncols + j];
        }  
        MPI_Send(buffer, ncols, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
        numsent++;
    }

    // Recieve the answers computed by the slave processes and append it to matrix c
    for (int i = 0; i < nrows; i++) {
        MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;
        anstype = status.MPI_TAG;
        c[anstype-1] = ans;

        // if test_output = 1, write output
        

        // if we haven't finished the job, keep sending more rows to the slave processes, until all rows are accounted for
        if (numsent < nrows) {
            for (int j = 0; j < ncols; j++) {
                buffer[j] = aa[numsent*ncols + j];
            }  
            MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD);
            numsent++;
        } else {
            // job done!
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
        FILE * out = open_output_file("log_mpi_inner_product.txt");
        FILE * out2 = open_output_file("log_mpi_ans.txt");
        while(1) {
            MPI_Recv(buffer, bCols, datatype, source, tag, mpi_comm, &status);
            if (status.MPI_TAG == 0) {
                break;
            }
            row = status.MPI_TAG;
            ans = 0.0;
            for (int j = 0; j < bCols; j++) {
                ans += buffer[j] * b[j];
                fprintf(out, "PROCESS_id: %d buffer: %f b_col: %f ANS: %f\n", process_id, buffer[j], b[j], ans);
            }
            // send answer to master, along with the row #
            fprintf(out2, "ANS: %f ", ans);
            MPI_Send(&ans, 1, datatype, source, row, mpi_comm);
        }
    }
}
