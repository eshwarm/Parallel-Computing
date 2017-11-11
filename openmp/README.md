Requirments:

Activate OpenMP in GCC by passing -fopenmp to the compiler and linker. (Note that if you omit thisparameter, the code will probably still compile. But its execution will be sequential.)
You can control the number of threads in OpenMP in two ways: the environment variable OMP NUM THREADS or by calling the omp set num threads function. Use the function call.
Loop scheduling in OpenMP can be controlled either by specifying schedule(runtime) in the parallel for construct and specifying a OMP SCHEDULE environment variable, or by using the omp set schedule function.

1 Reduce

reduce.cpp

The program that will generate a random array of integers. Then it will compute the sum
of the array in parallel using the OpenMP for loop construct. It measures the time computing the sum takes.
The program will control the length of the array, the number of thread and the scheduling policy. It Outputs the value of the reduction to stdout and the time taken to compute it to stderr.

2 Numerical Integration

numint.cpp

The program implements the numerical integration in parallel with the OpenMP loop construct. It Outputs the value of the integration to stdout and the time taken to compute it to stderr.

3 Prefix Sum

prefixsum.cpp

Implements a parallel function using OpenMP parallel loop constructs to compute the prefix sum
of an array.Output the time it took on stderr.

4 Merge Sort

mergesort.cpp

Implements a parallel function using OpenMP parallel loop constructs to perform merge sort on
an array of integer. Output the time it took on stderr.

