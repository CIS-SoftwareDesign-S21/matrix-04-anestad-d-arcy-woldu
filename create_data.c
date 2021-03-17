#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"

double calcTime(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), double *a, double *b, int n);
void print_matrix_to_file(FILE *ptr, double *a, double *b, int n);

int main(int argc, char* argv[]){
  struct timespec start;
  struct timespec end;
  struct timespec res;
  double *a, *b;

  FILE *cfPtr;
  if((cfPtr = fopen("data.txt", "w")) == NULL){
    puts("File cannot be opened");
  }else{
    if (argc == 3) {
	int n = get_matrix_size_from_file(argv[1]);
        a = read_matrix_from_file(argv[1]);
        b = read_matrix_from_file(argv[2]);
        print_matrix_to_file(cfPtr, a, b, n);
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
      fprintf(ptr, ", 0.000000"); //TODO MPI w/o OpenMP
      fprintf(ptr, ", 0.000000"); //TODO MPI w/ OpenMP
      fprintf(ptr, "\n");
}


double calcTime(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), double *a, double *b, int n){
    struct timespec start;
    struct timespec end;
    double *c = malloc(sizeof(double) * n * n);

    clock_gettime(CLOCK_REALTIME, &start);
    (*fun_ptr)(c,a,n,n,b,n,n);
    clock_gettime(CLOCK_REALTIME, &end);
    double time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
    return time;
}

