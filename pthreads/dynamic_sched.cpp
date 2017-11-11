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

int stop = 0;


//task variables
struct args{
    int start;
    int end;
    int i;
};

//Numerical Integration variables
typedef struct {
        int intensity;
        int functionid, granularity;
        float a, b, n ,sum;
	    std::string sync;
        } variables;

    variables my_var;
    pthread_mutex_t mutexsum, mutex_granularity;

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

    while(!stop){
      int start, end;
      //getting the task by using granularity
      pthread_mutex_lock (&mutex_granularity);
      if((my_arg->start + my_var.granularity) <= my_var.n){
        start = my_arg->start;
        end  = my_arg->end;
        my_arg->start = end;
        my_arg->end = my_arg->start + my_var.granularity;
      }else if((my_arg->start + my_var.granularity) > my_var.n){
        start = my_arg->start;
        end = my_var.n;
        stop = 1;
      }
     pthread_mutex_unlock (&mutex_granularity);
     //Numerical integration
      for (int i =start; i<end; i++){
        x = my_var.a+((float(i)+0.5) *((my_var.b-my_var.a)/my_var.n)) ;
        fun_return = get_function_value(x, my_var.intensity, functionid);
        pthread_mutex_lock (&mutexsum);
        my_var.sum = my_var.sum + (fun_return *((my_var.b-my_var.a)/my_var.n));
        pthread_mutex_unlock (&mutexsum);
      }
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
    while(!stop){
      int start, end;
      //getting the task by using granularity
      pthread_mutex_lock (&mutex_granularity);
      if((my_arg->start + my_var.granularity) <= my_var.n){
        start = my_arg->start;
        end  = my_arg->end;
        my_arg->start = end;
        my_arg->end = my_arg->start + my_var.granularity;
      }else if((my_arg->start + my_var.granularity) > my_var.n){
        start = my_arg->start;
        end = my_var.n;
        stop = 1;
      }
      pthread_mutex_unlock (&mutex_granularity);
      //Numerical integration
      for (int i=start; i<end; i++){
        x = my_var.a+((float(i)+0.5) *((my_var.b-my_var.a)/my_var.n)) ;
        fun_return = get_function_value(x, my_var.intensity, functionid);
        my_sum = my_sum + (fun_return *((my_var.b-my_var.a)/my_var.n));
      }
    }
    pthread_mutex_lock (&mutexsum);
    my_var.sum+= my_sum;
    pthread_mutex_unlock (&mutexsum);
    pthread_exit((void*) 0);
}

void *chunk_level(void *arg)
{
    //chunk level approach, aggregating to shared variable once per task
    //for each thread
    args *my_arg = (args*) arg;
    float x, fun_return;
    int functionid = my_var.functionid;
    while(!stop){
      int start, end;
      float my_sum = 0;
      //getting the task by using granularity
      pthread_mutex_lock (&mutex_granularity);
      if((my_arg->start + my_var.granularity) <= my_var.n){
        start = my_arg->start;
        end  = my_arg->end;
        my_arg->start = end;
        my_arg->end = my_arg->start + my_var.granularity;
      }else if((my_arg->start + my_var.granularity) > my_var.n){
        start = my_arg->start;
        end = my_var.n;
        stop = 1;
      }
      pthread_mutex_unlock (&mutex_granularity);
      for (int i=start; i<end; i++){
        x = my_var.a+((float(i)+0.5) *((my_var.b-my_var.a)/my_var.n)) ;
        fun_return = get_function_value(x, my_var.intensity, functionid);
        my_sum = my_sum + (fun_return *((my_var.b-my_var.a)/my_var.n));
      }
      pthread_mutex_lock (&mutexsum);
      my_var.sum+= my_sum;
      pthread_mutex_unlock (&mutexsum);

    }

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
  my_var.granularity = atoi(argv[8]);
  pthread_t th[nbthreads];
  struct args *my_args;
  my_args = (struct args*)malloc(sizeof(struct args));
  if (argc < 7) {
//    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
  pthread_mutex_init(&mutexsum, NULL);  //mutex intialisation
  pthread_mutex_init(&mutex_granularity, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int temp0 = 0;
  int temp1;
  if(my_var.sync == "thread"){
    my_args->start = 0;
    my_args->end = my_var.granularity;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    for(int i=0; i< nbthreads; i++){
      pthread_create(&th[i], &attr, thread_level, my_args);
    }

    pthread_attr_destroy(&attr);

    for(int i=0; i<nbthreads;i++){
      pthread_join(th[i], &status);
    }
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count()<<endl;
  }else if (my_var.sync == "iteration"){

    my_args->start = 0;
    my_args->end = my_var.granularity;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    for(int i=0; i< nbthreads; i++){
      pthread_create(&th[i], &attr, iteration_level, my_args);
    }
    pthread_attr_destroy(&attr);
    for(int i=0; i<nbthreads;i++){
      pthread_join(th[i], &status);
    }
    free(my_args); //free memory allocation
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count()<<endl;
  }else if (my_var.sync == "chunk"){
    my_args->start = 0;
    my_args->end = my_var.granularity;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    for(int i=0; i< nbthreads; i++){
      pthread_create(&th[i], &attr, chunk_level, my_args);
    }
    pthread_attr_destroy(&attr);

    for(int i=0; i<nbthreads;i++){
      pthread_join(th[i], &status);
    }
    free(my_args);
    std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count()<<endl;
  }
  std::cout<< my_var.sum <<endl;
  pthread_mutex_destroy(&mutexsum);
  pthread_mutex_destroy(&mutex_granularity);
  pthread_exit(NULL);
  return 0;
}
