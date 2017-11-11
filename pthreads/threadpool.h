#include <pthread.h>
#include<iostream.h>

using namespace std;


class ThreadPool
{
public:
  ThreadPool(int pool_size);
private:
  int m_pool_size;
  volatile int m_pool_state;
}

