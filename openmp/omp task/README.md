
Activate OpenMP in GCC by passing -fopenmp to the compiler and linker. (Note that if you omit this parameter, the code will probably still compile. But its execution will be sequential.)
You can control the number of threads in OpenMP in two ways: the environment variable OMP NUM THREADS or by calling the omp set num threads function.
When recursively decomposing the workload, the granularity of the decomposition is set by the size of the smallest task that will not be further decomposed. Traditionally, a threshold is used under which all tasks are processed sequentially.
1 Reduce

reduce.cpp Implements computing the sum of an array using the OpenMP task decomposition.

2 Merge Sort

merge.cpp Implements a parallel function using OpenMP tasks to perform merge sort on an array of integer.

3 Longest Common Subsequence

lcs.cpp Implements a parallel version of the Longest Common Subsequence algorithm. 

4 Bubble Sort

bubble.cpp Implements a parallel version of BubbleSort algorithm.