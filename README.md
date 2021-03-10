# matrix-04-anestad-d-arcy-woldu
matrix-04-anestad-d-arcy-woldu created by GitHub Classroom


### What is SIMD, OMP, and MPI? What are the differences between them?


SIMD: Single Instruction Multiple Data is an architecture where an instruction performs the same operation on multiple streams of data (vectors). Proccesses share one global memory, where data is stored.

OMP: Parallel programs are executed on a local shared memory system. Threads have access to shared data. 

MPI: Message Passing Interface is mainly useful used in a distributed system. Tasks are divided and sent between different nodes on the cluster. Memory is private between these nodes. Simple programs can take a longer time to execute due to overhead such as networking.
