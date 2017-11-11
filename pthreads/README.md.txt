Requirments:

Link with the pthread library by passing -lpthread.

Writing a static loop scheduler

A static loop scheduler is the simplest way to achieve some parallelism. We will use it to make the numerical integration problem parallel.

Essentially, with T threads, each thread will do exactly  of the loop iterations. So that if the loop has 100 iterations and there are four threads, the rest threads will execute loop iterations from 0 to 24, the second will execute iterations from 25 to 49, ...

With the pthread library, one can pass parameters to a thread by passing a pointer to a struct that
contains all the information it needs. The simplest way to write this code is to have one instance of the struct per thread, to give each thread the different information it needs. 
Some are the same for all the threads, a, b, a pointer to the results. Some are different, such as the range of indices they are supposed to execute.
Note that all the threads will need access to a pointer to the float that stores the sum of all the values.

To avoid race conditions, this variables needs to be placed under mutual exclusion and there are two ways
to do this:
 iteration-level mutual exclusion essentially puts the mutual exclusion on each access to the variable
 thread-level mutual exclusion makes each threads locally compute the sum of values in its own variable, and only aggregates the value into the results once all the iterations have been computed.

The code in static sched.cpp is for a static loop scheduler and use it to compute numerical integration. The code so that it outputs the integral value on stdout and the time it takes to make
the computation on stderr.

The program will take the following command line parameters:
functionid, same as in numerical integration assignment
 a, same as in numerical integration assignment
 b, same as in numerical integration assignment
 n, same as in numerical integration assignment
 intensity, same as in numerical integration assignment
 nbthreads, an integer indicating the number of threads that should perform the numerical integration
 sync, a string, either iteration or thread that indicate which synchronization method to use.

Writing a dynamic loop scheduler
A dynamic loop schedulers are essentially managed by distributing ranges of indices to threads when they
request them.

The worker threads execute a code that looks like

whi l e ( ! loop . done ( ) ) 
i n t begin , end ;
loop . ge tnext (&begin , &end ) ;
executeinnerloop( begin , end ) ;

The implementation of a loop scheduler boils done to implementing the two functions done and getnext.
They can be easily implemented using mutual exclusion.
The size of the interval [begin;end] is called the granularity and is usually a parameter of the scheduler. Managing the memory of the loop scheduler can be a bit tricky as we need to be sure
that all threads coordinated by the loop scheduler are done.
To compute numerical integration, note that the program needs to make sure that the result is correct,
to do so, I enforced the mutual exclusion in three places:
 iteration, Within the most inner loop for each step of the numerical integration,
 chunk, By locally storing the value in execute inner loop and adding them to a shared variable once
per call.
 thread, By storing one value per thread, and aggregating to the shared variable once it is done.
The code should take the same parameter as the previous one except:
 sync can also take chunk as a synchronization method
 granularity, an integer indicating how many iterations does one thread take at a time.
