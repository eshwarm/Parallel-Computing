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
/* merge the two halves  of array arr[] */
void merge(int *arr, int l, int m, int r);
 
// minimum of two numbers function
int minimum(int x, int y) { return (x<y)? x :y; }

// Iterative mergesort
void mergeSort(int *arr, int n)
{
   int sub_arr_size;  //current size of subarrays to be merged
   int left_start; // starting index of left subarray
                   
 
   // Merge subarrays in bottom up manner.
   for (sub_arr_size=1; sub_arr_size<=n-1; sub_arr_size = 2*sub_arr_size)
   {
    #pragma omp parallel
    {	 
	  #pragma omp for schedule(static)
         for (left_start=0; left_start<n-1; left_start += 2*sub_arr_size)
         {
           int mid = minimum(left_start + sub_arr_size - 1,n-1);	
           int right_end = minimum(left_start + 2*sub_arr_size - 1, n-1);
           // Merge Subarrays arr[left_start...mid] & arr[mid+1...right_end]
           merge(arr, left_start, mid, right_end);
         }
    }
   }
}
 
void merge(int *arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    // create temp arrays 
	  int * L = new int [n1];
	  int * R = new int [n2];
 
    // Copy data to temp arrays L and R 
    for (i = 0; i < n1; i++)
        *(L+i) = *(arr+l + i);
    for (j = 0; j < n2; j++)
        *(R+j) = *(arr+m + 1+ j);
 
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
  mergeSort(arr, atoi(argv[1]));    
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;  
  checkMergeSortResult (arr, atoi(argv[1]));
  
  delete[] arr;

  return 0;
}
