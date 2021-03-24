# Matrix Multiplication Lab: Anestad, D'Arcy, and Woldu
"matrix-04-anestad-d-arcy-woldu" repository created by GitHub Classroom


### What is SIMD, OMP, and MPI? What are the differences between them?


SIMD: Single Instruction Multiple Data is an architecture where an instruction performs the same operation on multiple streams of data (vectors). Proccesses share one global memory, where data is stored.

OMP: Parallel programs are executed on a local shared memory system. Threads have access to shared data. 

MPI: Message Passing Interface is mainly useful used in a distributed system. Tasks are divided and sent between different nodes on the cluster. Memory is private between these nodes. Simple programs can take a longer time to execute due to overhead such as networking.

Michael D'Arcy Writting Contribution:

Teamwork: 
Throughout the entire project, I was in charge of developing some of the project board, implementing the OpenMP multiplication functionality to the program, rewriting our data creation class to handle data input from files, and re-creating the SIMD vs an unoptimized algorithm portion of the program using Java. 
For the project board I created a lot of tasks that we were to finish within the first two weeks. We used the project board to distribute tasks throughout the group. I would then move the tasks into their proper positions based on the submittions from all the group members. After each week we would then review the updated project board and reassign tasks that needed to be completed.
Secondly I worked on the OpenMP implementation in our program. I looked into OpenMP and how it worked by using the lab slides as well as lecture slides from our class. In the end, a relatively fully functional version of the OpenMP algorithm was available to be used from the mmult_omp.c. I then incorperated the function within the mmult_omp.c file into our created file, create_data.c. This file would then graph the time usage data of the OpenMP algorithm alongside the time data of the remaining required algorithms.
Then I started to work on the file input implementation. I decided to use file input to get text data from outside the program and to allow the user to get the resulting product data. I ended up allowing users to add the files using the command line arguments that are used to run the data creation executable. A user will either be able to include no additional arguments to the executable in order to get data based on randomly generated matrices or they will include two file paths as a second and third argument. The matrix data from within the two included files would be multiplied together and their output would be stored within the output file. I also changed our data creation code again in order to reduce the amount of duplicated code. This allowed further edits to the code to be easier and quicker.
Lastly I was in charge of using parallel programming practices in Java to create similar output to our C program. I firstly created a Java program that would contain various functions in order to generate a random matrix, compare two matrices, and print out provided matrices to the user. I then created a function that would use the provided unoptimized algorithm code to multiply two matrices together and return the resulting matrix. The first implementation of this used a multi-dimensional array instead of a regular array which resulted in the unoptimized algorithm to produce the correct answer faster than the "optimized" algorithm. After the array changes were finalized, I created a different function that would use the parallelism that you gain from Threads. This function used a more optimized iterative design to multiply matrices together as a base and then Threads were used to divide the workload onto different processes. While in reality forking the main process into different processes and then using the different cores of the machine to further increase the optimization, the Thread implementation resulted in a measurable decrease in the function's operation time.
