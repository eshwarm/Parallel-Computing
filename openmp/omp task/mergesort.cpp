#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

  
#ifdef __cplusplus
}
#endif
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int *arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    // create temp arrays 
	  int * L = new int [n1];
	  int * R = new int [n2];
 
    // Copy data to temp arrays L and R 
  #pragma omp parallel
  {	 
	#pragma omp for nowait schedule(runtime)	
    for (i = 0; i < n1; i++)
        *(L+i) = *(arr+l + i);
  }
  #pragma omp parallel
  {	  
    #pragma omp for nowait schedule(runtime)
    for (j = 0; j < n2; j++)
        *(R+j) = *(arr+m + 1+ j);
  }
    // Merge the temp arrays back into arr[l..r]
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (*(L+i) <= *(R+j))
        {
            *(arr+k) = *(L+i);
            i++;
        }
        else
        {
            *(arr+k) = *(R+j);
            j++;
        }
        k++;
    }
 
    // Copy the remaining elements of L
    while (i < n1)
    {
        *(arr+k) = *(L+i);
        i++;
        k++;
    }
 
    // Copy the remaining elements of R
    while (j < n2)
    {
        *(arr+k) = *(R+j);
        j++;
        k++;
    }
	  delete[] R;
	  delete[] L;
}
 
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int *arr, int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
		
        int m = l+(r-l)/2;
 
        // Sort first and second halves
		#pragma omp task firstprivate (arr, l, m)
        mergeSort(arr, l, m);
		#pragma omp task firstprivate (arr,m, r)
        mergeSort(arr, m+1, r);
		#pragma omp taskwait
        merge(arr, l, m, r);
    }
}

int main (int argc, char* argv[]) {
  int num_threads = atoi(argv[2]);
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];

  generateMergeSortData (arr, atoi(argv[1]));
  omp_set_num_threads(num_threads);
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  #pragma omp parallel
  {
    #pragma omp single
    mergeSort(arr, 0, atoi(argv[1])-1);    
  }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;  
  checkMergeSortResult (arr, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}
