// C++ program for implementation of Bubble sort
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

// A function to implement bubble sort using For reduction
void bubbleSort(int *arr, int n){
	int  n, temp,  flag = 1;  //flag to break the loop
    while(flag){
    #pragma omp parallel private(temp)
    {       
      flag = 0;
	  //Take even elements of array
      #pragma omp for reduction(+:flag)
      for(int i = 0; i < n - 1; i = i + 2){
            if(arr[i] > arr[i+1] ){
                temp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = temp;
                ++flag;
           }
        }
	  //Take od elements	
      #pragma omp for reduction(+:flag)
      for(int i = 1; i < n - 1; i = i + 2){
        if( arr[i] > arr[i+1] ){
			temp = arr[i];
            arr[i] = arr[i+1];
            arr[i+1] = temp;
            ++flag;
        }
      }
    }
   }

}


int main(int argc, char* argv[])
{
  int num_threads = atoi(argv[2]);  //get no of threads
  int n = atoi(argv[1]); // get array size
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
  
  if (argc < 2) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }	

  int * arr = new int [atoi(argv[1])];
  generateMergeSortData (arr, atoi(argv[1]));  //Populate buble sort data
  omp_set_num_threads(num_threads);
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  bubbleSort(arr, n);
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;  
  checkMergeSortResult (arr, atoi(argv[1]));	//Check bubblesort results
  delete[] arr;
  return 0;
}