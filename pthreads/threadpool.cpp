#include<threadpool.h>
#include<global.h>

ThreadPool::ThreadPool(int pool_size) m_pool_size(pool_size)

int ThreadPool::initialize_threadpool(){
m_pool_state = STARTED;

}

