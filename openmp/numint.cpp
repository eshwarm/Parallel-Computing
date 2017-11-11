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
using namespace std;

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float get_function_value(float x, int intensity, int functionid){
    //return the output of function
    switch(functionid){
      case 1:
	return f1(x, intensity);
      case 2:
	return f2(x, intensity);
      case 3:
	return f3(x, intensity);
      case 4:
	return f4(x, intensity);
    }
}

int main (int argc, char* argv[]) {
  
  int intensity, num_threads;
  int functionid, ganularity, n;
  float a, b,sum = 0,fun_return;
  float  x;
  std::string sched_type;	
  functionid = atoi(argv[1]);
  a = atof(argv[2]);
  b = atof(argv[3]);
  n = atoi(argv[4]);
  intensity = atoi(argv[5]);
  num_threads = atoi(argv[6]);
  sched_type = argv[7];
  ganularity = atoi(argv[8]);

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
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }
  
  omp_set_num_threads(num_threads); //set number of threads for parallel
    //set schedule type using omp fun
  if(sched_type == "static"){
	  omp_set_schedule(omp_sched_static, ganularity);
  }else if (sched_type == "dynamic"){
	  omp_set_schedule(omp_sched_dynamic, ganularity);
  }else if (sched_type == "guided"){
	  omp_set_schedule(omp_sched_guided, ganularity);
  }

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  //Numerical itegration computation using for reduction construct
  #pragma omp parallel 
  {	 
  #pragma omp for reduction(+:sum) schedule(runtime) 
  for (int i =0; i<n; i++){
	  x = a+((float(i)+0.5) *((b-a)/n)) ;
	  fun_return = f1(x, intensity);
	  sum += (fun_return  *((b-a)/n));
    }
  }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<< elapsed_seconds.count()<<endl;
  
  std::cout<< sum <<endl;
  return 0;

}
