#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

#include "mat.h"


void write_matrices(FILE *ptr_a, FILE *ptr_b, double *a, double *b, int N);

int main(int argc, char* argv[]) {

        FILE *f_ptr_a;
        FILE *f_ptr_b;
        double *a, *b;

        int n[] = {1,2,3,4,5,10,20,50,100,200,300,400,500};
        for(int i = 0; i < 13; i++) {

                char buffer_a[500]; // The filename buffer.
                char buffer_b[500]; // The filename buffer.
                
                sprintf (buffer_a, "input/a/a_%d.txt", n[i]);
                sprintf (buffer_b, "input/b/b_%d.txt", n[i]);

                if((f_ptr_a = fopen(buffer_a, "w")) == NULL){
                        puts("Matrices A gen file cannot be opened");
                        return 0;
                }

                if((f_ptr_b = fopen(buffer_b, "w")) == NULL){
                        puts("Matrices B gen file cannot be opened");
                        return 0;
                }

                a = gen_matrix(n[i], n[i]);
                b = gen_matrix(n[i], n[i]);

                write_matrices(f_ptr_a, f_ptr_b, a, b, n[i]);

                fclose(f_ptr_a);
                fclose(f_ptr_b);
        }
        // FOR TESTING PURPOSES, Will be moved later when actual test units are created
        // int N = get_matrix_size_from_file("input/a/a_50.txt");
        // double *test = read_matrix_from_file("input/a/a_50.txt");
        // for (int i = 0 ;i<N*N; i++) {
        //         printf("%0.2f\n", test[i]);
        // }
}

void write_matrices(FILE *ptr_a, FILE *ptr_b, double *a, double *b, int N) {
        fprintf(ptr_a, "%d %d", N, N);
        for (int i = 0; i < N*N; i++) {
                if( (i % N*N) == 0) {
                        fprintf(ptr_a, "\n%0.2f ", a[i] * 100);
                }
                else {
                        fprintf(ptr_a, "%0.2f ", a[i] * 100);
                }         
        }
        fprintf(ptr_b, "%d %d\n", N, N);
        for (int i = 0; i < N*N; i++) {
                if( (i % N*N) == 0) {
                        fprintf(ptr_b, "\n%0.2f ", b[i] * 100);
                }
                else {
                        fprintf(ptr_b, "%0.2f ", b[i] * 100);
                }  
        }
}
