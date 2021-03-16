PGMS=create_data mmult_omp_timing matrix_times_vector hello test_mmult mxv_omp_mpi mmult_mpi_omp

all:	${PGMS}

mmult_mpi_omp:		mmult.o mmult_mpi_omp.o mat.c
	mpicc -o mmult_mpi_omp -fopenmp -O3 mmult.o mmult_mpi_omp.o mat.c

mmult_mpi_omp.o:	mmult_mpi_omp.c
	mpicc -c -fopenmp -O3 mmult_mpi_omp.c

mmult_omp_timing:	mmult.o mmult_omp.o mmult_omp_timing.o mat.c
	gcc -o mmult -fopenmp -O3 mmult.o mmult_omp.o mmult_omp_timing.o mat.c -o mmult_omp_timing

mat.o:	mat.c
	gcc -c mat.c 

mmult.o:	mmult.c
	gcc -c mmult.c

mmult_omp.o:	mmult_omp.c
	gcc -c -fopenmp mmult_omp.c

mmult_omp_timing.o:	mmult_omp_timing.c
	gcc -c  mmult_omp_timing.c

create_data.o: create_data.c
	gcc -c create_data.c

create_data:	mmult.o mmult_simd.o mmult_simd_O3.o mmult_omp.o create_data.o mat.c
	gcc -o mmult mmult.o -fopenmp mmult_simd.o mmult_simd_O3.o mmult_omp.o create_data.o mat.c -o create_data

matrix_times_vector:	matrix_times_vector.c mat.c
	mpicc -O3 -o matrix_times_vector matrix_times_vector.c mat.c

hello:	hello.c
	mpicc -O3 -o hello hello.c

mxv_omp_mpi:	mxv_omp_mpi.c mat.c
	mpicc -fopenmp -O3 -o mxv_omp_mpi mxv_omp_mpi.c mat.c

test_mmult:	test_mmult.c mmult.c mat.c
	gcc test_mmult.c mmult.c mat.c -lm -o test_mmult

mmult_simd.o:   mmult_simd.c
	gcc -c mmult_simd.c

mmult_simd_O3.o: mmult_simd_O3.c
	gcc -c -O3 mmult_simd_O3.c

clean:
	rm -f *.o
	rm -f ${PGMS}

