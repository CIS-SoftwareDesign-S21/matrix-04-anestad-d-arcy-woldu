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

double mmult_mpi(int argc, char* argv[], double *aa, double *b) {
    int nrows, ncols;
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

    if (argc > 0) {
        nrows = atoi(argv[1]);
        ncols = nrows;
        // aa = (double*)malloc(sizeof(double) * nrows * ncols);
        b = (double*)malloc(sizeof(double) * ncols);
        c = (double*)malloc(sizeof(double) * nrows);
        buffer = (double*)malloc(sizeof(double) * ncols);
        master = 0;

        if (myid == master) {
            master_code(aa, b, c, buffer, ans, nrows, ncols, master, numprocs, status, &total_times);
        } else {
            MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
            compute_inner_product(buffer, ncols, MPI_DOUBLE, master, 
                                    MPI_ANY_TAG, MPI_COMM_WORLD, status,
                                    myid, nrows, b, row, ans);
        }
        
    } else {
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }

    MPI_Finalize();
    return total_times;
}

FILE * open_output_file(const char * path) {
    FILE *cfPtr;
    if((cfPtr = fopen(path, "w")) == NULL) {
        puts("File cannot be opened");
    }
    return cfPtr;  
}

int main(int argc, char **argv) {   

    double delta_t;
    double *a, *b;
    FILE *output_ptr;
    int n, m;

    // n = atoi(argv[1]);
    // m = atoi(argv[1]);
    // a = gen_matrix(m, n);
    // b = gen_matrix(m, n);
    
    // output_ptr = open_output_file("output/mpi_output.txt");
    // delta_t = mmult_mpi(argc, argv, a, b);

    // fprintf(output_ptr, "%d", n);
    // fprintf(output_ptr, ", %f\n", delta_t);
    // fclose(output_ptr);

    if(argc == 3) {
        // matrices a and b provided
        output_ptr = open_output_file("output/mpi_output.txt");
        
        n = get_matrix_size_from_file(argv[1]);
        a = read_matrix_from_file(argv[1]);
        b = read_matrix_from_file(argv[2]);
        delta_t = mmult_mpi(argc, argv, a, b);

        fprintf(output_ptr, "%d", n);
        fprintf(output_ptr, ", %f\n", delta_t);
        fclose(output_ptr);

    }
    else if(argc == 2) {
        // business as usual, gen a random square matrices of size argv[1]

        output_ptr = open_output_file("output/mpi_output.txt");
        n = atoi(argv[1]);
        m = n;

        a = gen_matrix(m, n);
        b = gen_matrix(m, n);
        mmult_mpi(argc, argv, a, b);

        fprintf(output_ptr, "%d", n);
        fprintf(output_ptr, ", %f\n", delta_t);
        fclose(output_ptr);

    }
    else {
        // no matrices provided (pass a default value)
        // run through the matrices stored in the input dir

        int N[] = {1,2,3,4,5,10,20,50,100,200,300,400,500};
        for(int i = 0; i < N; i++) {

            output_ptr = open_output_file("output/mpi_output.txt");

            char buffer_a[500]; // The filename buffer.
            char buffer_b[500]; // The filename buffer.
            sprintf (buffer_a, "input/a/a_%d.txt", N[i]);
            sprintf (buffer_b, "input/b/b_%d.txt", N[i]);

            a = read_matrix_from_file(buffer_a);
            b = read_matrix_from_file(buffer_b);
            delta_t = mmult_mpi(argc, argv, a, b);

            fprintf(output_ptr, "%d", N[i]);
            fprintf(output_ptr, ", %f\n", delta_t);
            fclose(output_ptr);
        }
    }
    return 0;
}


void master_code(double *aa, double *b, double *c, double *buffer, double ans, int nrows, int ncols, int master, int numprocs,
                  MPI_Status status, int *total_times) {
                    
    double starttime, endtime;
    int anstype, numsent, sender;

    // Master Code goes here
    // aa = gen_matrix(nrows, ncols);
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
    total_times = endtime - starttime;
    printf("%f\n",total_times);
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
