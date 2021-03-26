#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"

double calcTime(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), double *a, double *b, int n);
void print_matrix_to_file(FILE *ptr, double *a, double *b, int n);
void write_test_output(double *a, double *b, int n);
void write_output(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), 
                FILE *ptr, double *a, double *b, double *c, int n);

FILE * open_output_file(const char * path);

int main(int argc, char* argv[]) {

  struct timespec start;
  struct timespec end;
  struct timespec res;
  double *a, *b;

  FILE *cfPtr;
  if((cfPtr = fopen("data.txt", "w")) == NULL){
    puts("File cannot be opened");

  } else {

    if (argc == 3) {
      int n = get_matrix_size_from_file(argv[1]);
      a = read_matrix_from_file(argv[1]);
      b = read_matrix_from_file(argv[2]);
      print_matrix_to_file(cfPtr, a, b, n);

      // create test_files here
      write_test_output(a, b, n);

    } else if (argc != 1) {
      puts("create_data <file_1> <file_2>");

    } else {

      int n[] = {1,2,3,4,5,10,20,50,100,200,300,400,500};

      for(int i = 0; i < 13; i++){
        a = gen_matrix(n[i], n[i]);
        b = gen_matrix(n[i], n[i]);
        print_matrix_to_file(cfPtr, a, b, n[i]);
      }
    }
  }
}

void print_matrix_to_file(FILE *ptr, double *a, double *b, int n) {
      fprintf(ptr, "%d", n);
      fprintf(ptr, ", %f", calcTime(mmult,         a, b, n));
      fprintf(ptr, ", %f", calcTime(mmult_simd,    a, b, n));
      fprintf(ptr, ", %f", calcTime(mmult_simd_O3, a, b, n));
      fprintf(ptr, ", %f", calcTime(mmult_omp,     a, b, n));
      fprintf(ptr, ", 0.000000"); //TODO MPI w/ OpenMP
      fprintf(ptr, "\n");
}


double calcTime(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), double *a, double *b, int n) {
    struct timespec start;
    struct timespec end;
    double *c = malloc(sizeof(double) * n * n);

    clock_gettime(CLOCK_REALTIME, &start);
    (*fun_ptr)(c,a,n,n,b,n,n);
    clock_gettime(CLOCK_REALTIME, &end);
    double time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    return time;
}

void write_test_output(double *a, double *b, int n) {
  FILE *reg_fptr = open_output_file("output/regular_mmult_product.txt");
  FILE *simd_fptr = open_output_file("output/simd_product.txt");
  FILE *simdo3_fptr = open_output_file("output/simd_o3_product.txt");
  FILE *omp_fptr = open_output_file("output/omp_product.txt");

  double *reg_output = malloc(sizeof(double) * n * n);
  double *simd_output = malloc(sizeof(double) * n * n);
  double *simdo3_output = malloc(sizeof(double) * n * n);
  double *omp_output = malloc(sizeof(double) * n * n);

  write_output(mmult, reg_fptr, a, b, reg_output, n);
  write_output(mmult_simd, reg_fptr, a, b, reg_output, n);
  write_output(mmult_simd_O3, reg_fptr, a, b, reg_output, n);
  write_output(mmult_omp, reg_fptr, a, b, reg_output, n);

}

void write_output(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), 
                FILE *ptr, double *a, double *b, double *c, int n) {

                (*fun_ptr)(c,a,n,n,b,n,n);
                for (int i = 0; i < sizeof(c) / c[0]; i++) {
                    fprintf(ptr, "%f ", c[i]);
                }
                fclose(ptr);
}

// void gen_mmult_test_file(FILE *ptr, double *a, double *b, double *c) {

// }
// void gen_mmultSIMD_test_file(FILE *ptr, double *a, double *b, double *c) {

// }
// void gen_mmultSIMDO3_test_file(FILE *ptr, double *a, double *b, double *c) {

// }
// void gen_mmultOMP_test_file(FILE *ptr, double *a, double *b, double *c){

// }

FILE * open_output_file(const char * path) {
    FILE *cfPtr;
    if((cfPtr = fopen(path, "a")) == NULL) {
        puts("File cannot be opened");
    }
    return cfPtr;  
}
