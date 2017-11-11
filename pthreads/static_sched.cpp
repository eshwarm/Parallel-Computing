#define _GLIBCXX_USE_CXX11_ABI 0
#include <iostream>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif
using namespace std;

//task variables
struct args{
    int start;
    int end;
    int i;
};

//Numerical Integration variables
typedef struct {
        int intensity;
        int functionid;
        float a, b, n ,sum;
	    std::string sync;
        } variables;

    variables my_var;
    pthread_mutex_t mutexsum; //mutex instantiation

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

void *iteration_level(void *arg){
    //Iteration level approach, sum is done at each level of
    //Numerical Integration
    args *my_arg = (args*) arg;
    float my_sum = 0;
    float x, fun_return;
    int functionid = my_var.functionid;
    //Numerical integration
    for (int i =my_arg->start; i<my_arg->end; i++){
      x = my_var.a+((float(i)+0.5) *((my_var.b-my_var.a)/my_var.n)) ;
      fun_return = get_function_value(x, my_var.intensity, functionid);
      pthread_mutex_lock (&mutexsum);
      my_var.sum = my_var.sum + (fun_return *((my_var.b-my_var.a)/my_var.n));
      pthread_mutex_unlock (&mutexsum);
    }
}

void *thread_level(void *arg)
{
    //Thread level approach, aggregating shared variable sum after
    //all tasks are done
    args *my_arg = (args*) arg;
    float my_sum = 0;
    float x, fun_return;
    int functionid = my_var.functionid;
    for (int i =my_arg->start; i<my_arg->end; i++){
      x = my_var.a+((float(i)+0.5) *((my_var.b-my_var.a)/my_var.n)) ;
      fun_return = get_function_value(x, my_var.intensity, functionid);
      my_sum = my_sum + (fun_return *((my_var.b-my_var.a)/my_var.n));
    }
    pthread_mutex_lock (&mutexsum);
    my_var.sum+= my_sum;
    pthread_mutex_unlock (&mutexsum);
    pthread_exit((void*) 0);
}


int main (int argc, char* argv[]) {

  int  nbthreads;
  my_var.sum = 0;
  void *status;
  pthread_attr_t attr;

  my_var.functionid = atoi(argv[1]);
  my_var.a = atof(argv[2]);
  my_var.b = atof(argv[3]);
  my_var.n = atof(argv[4]);
  my_var.intensity = atoi(argv[5]);
  nbthreads = atoi(argv[6]);
  my_var.sync = argv[7];
  pthread_t th[nbthreads];
  if (argc < 7) {
//    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  pthread_mutex_init(&mutexsum, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int temp0 = 0;
  int temp1;
  if(my_var.sync == "thread"){
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    for(int i=0; i< nbthreads; i++){
      args *arg = new args();
      arg->i = i;
      arg->start = temp0;
      temp1 = (my_var.n/nbthreads) * (i+1); // temp1 variable to get end value for later use
      arg->end = temp1;
      temp0 = temp1;  //start value
      pthread_create(&th[i], &attr, thread_level, arg);
    }
    pthread_attr_destroy(&attr);  //Destroy attribute after usage
    //Thread join
    for(int i=0; i<nbthreads;i++){
      pthread_join(th[i], &status);
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count()<<endl;
  }else if (my_var.sync == "iteration"){
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    for(int i=0; i< nbthreads; i++){
      args *arg = new args();
      arg->i = i;
      arg->start = temp0;
      temp1 = (my_var.n/nbthreads) * (i+1);
      arg->end = temp1;
      temp0 = temp1;
      pthread_create(&th[i], &attr, iteration_level, arg);
    }
    pthread_attr_destroy(&attr);
    //Thread Join
    for(int i=0; i<nbthreads;i++){
      pthread_join(th[i], &status);
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count()<<endl;
  }
  std::cout<< my_var.sum <<endl;
  pthread_mutex_destroy(&mutexsum);  //destroy mutex once done
  pthread_exit(NULL);
  return 0;
}
