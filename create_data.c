#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"

int main(int argc){
  struct timespec start;
  struct timespec end;
  struct timespec res;
  double *a, *b, *c1, *c2;

  FILE *cfPtr;
  if((cfPtr = fopen("data.txt", "w")) == NULL){
    puts("File cannot be opened");
  }else{
    int n[] = {1,2,3,4,5,10,20,50,100,200,300,400,500};
    for(int i = 0; i < 13; i++){
      double times[3];
      a = gen_matrix(n[i], n[i]);
      b = gen_matrix(n[i], n[i]);
      c1 = malloc(sizeof(double) * n[i] * n[i]);
      c2 = malloc(sizeof(double) * n[i] * n[i]);

      clock_gettime(CLOCK_REALTIME, &start);
      mmult(c1, a, n[i], n[i], b, n[i], n[i]);
      clock_gettime(CLOCK_REALTIME, &end);
      times[0] = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
      fprintf(cfPtr, "%d, %f, ", n[i], times[0]);

      clock_gettime(CLOCK_REALTIME, &start);
      mmult_simd(c2, a, n[i], n[i], b, n[i], n[i]);
      clock_gettime(CLOCK_REALTIME, &end);
      times[1] = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
      fprintf(cfPtr, "%f, ", times[1]);

      clock_gettime(CLOCK_REALTIME, &start);
      mmult_simd_O3(c2, a, n[i], n[i], b, n[i], n[i]);
      clock_gettime(CLOCK_REALTIME, &end);
      times[2] = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
      fprintf(cfPtr, "%f\n", times[2]);


    }
  }
}
