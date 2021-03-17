#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"

double calcTime(int (*fun_ptr)(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols), double *a, double *b, int n);

int main(int argc){
  struct timespec start;
  struct timespec end;
  struct timespec res;
  double *a, *b;

  FILE *cfPtr;
  if((cfPtr = fopen("data.txt", "w")) == NULL){
    puts("File cannot be opened");
  }else{
    int n[] = {1,2,3,4,5,10,20,50,100,200,300,400,500};
    for(int i = 0; i < 13; i++){
      double times[3];
      a = gen_matrix(n[i], n[i]);
      b = gen_matrix(n[i], n[i]);

      fprintf(cfPtr, "%d", n[i]);

      fprintf(cfPtr, ", %f", calcTime(mmult,         a, b, n[i]));
      fprintf(cfPtr, ", %f", calcTime(mmult_simd,    a, b, n[i]));
      fprintf(cfPtr, ", %f", calcTime(mmult_simd_O3, a, b, n[i]));
      fprintf(cfPtr, ", %f", calcTime(mmult_omp,     a, b, n[i]));
      fprintf(cfPtr, ", 0.000000"); //TODO MPI w/o OpenMP
      fprintf(cfPtr, ", 0.000000"); //TODO MPI w/ OpenMP

      fprintf(cfPtr, "\n");
    }
  }
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

