#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>


#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif
using namespace std;

int main (int argc, char* argv[]) {
  double sum = 0.0;	
  int n = atoi(argv[1]);
  int num_threads = atoi(argv[2]);
  
  std::string sched_type = argv[3]; //get schedul type
  int ganularity = atoi(argv[4]); //get ganularity
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
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];

  generateReduceData (arr, atoi(argv[1]));
  omp_set_num_threads(num_threads);  //set number of threads for parallel
  
  //set schedule type using omp fun
  if(sched_type == "static"){
	  omp_set_schedule(omp_sched_static, ganularity);
  }else if (sched_type == "dynamic"){
	  omp_set_schedule(omp_sched_dynamic, ganularity);
  }else if (sched_type == "guided"){
	  omp_set_schedule(omp_sched_guided, ganularity);
  }
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  #pragma omp parallel
  {	 
	#pragma omp for reduction(+:sum) schedule(runtime)
					
	for(int i=0;i<n; i++){
	 sum+=*(arr+i);
	}
  }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;
  
  std::cout<< sum <<endl;
  



  
  delete[] arr;

  return 0;
}
