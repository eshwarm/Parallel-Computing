#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
  
  
#ifdef __cplusplus
}
#endif

//Prefix sum computation function
void prefixsum_inplace(int *arr,int *pr, int n, int nthreads) {
    int *thread_sum;
	omp_set_num_threads(nthreads); //set number of threads for parallel
    thread_sum = new int[nthreads+1];
    thread_sum[0] = 0;  //initialising sum to 0
    #pragma omp parallel
    {
        const int threadi = omp_get_thread_num();
        int sum = 0;
		//computes prefix sum for each chunk per thread
        #pragma omp for schedule(static)
        for (int i=0; i<n; i++) {
            sum += arr[i];
            pr[i+1] = sum;
        }
        thread_sum[threadi+1] = sum;
		
		//Takes each final sum of chunk and adds to temp variable per thread
        #pragma omp barrier
        int temp = 0;
        for(int i=0; i<(threadi+1); i++) {
            temp += thread_sum[i];
        }
		//take the temp variable sum and adds it to numbers of next thread chunk
        #pragma omp for schedule(static)
        for (int i=0; i<n; i++) {
            pr[i+1] += temp;
        }
    }
    delete[] thread_sum;
}

int main (int argc, char* argv[]) {
	int nthreads = atoi(argv[2]);

  //forces openmp to create the threads beforehand
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
	int *pr = new int [atoi(argv[1])+1];
	pr[0] = 0;  

  generatePrefixSumData (arr, atoi(argv[1]));
  
  //write code here
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  prefixsum_inplace(arr,pr, atoi(argv[1]), nthreads);
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;
  checkPrefixSumResult(pr, atoi(argv[1]));
  
  delete[] arr;
  delete[] pr;

  return 0;
}
