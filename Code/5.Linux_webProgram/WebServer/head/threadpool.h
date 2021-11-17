#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include "locker.h"
#include <iostream>

//线程池模板类，代码复用
template <typename T>
class threadpool
{
private:
    //线程的数量
    int m_thread_number;
    // 线程池数组，大小为m_thread_number
    pthread_t * m_threads;

    //请求队列最多允许的，等待处理的请求数量

    int m_max_requests;
    //请求队列
    std::list<T*> m_workqueue;

    //互斥锁
    locker m_queuelocker;

    //信号量来判断是否有任务需要处理
    sem m_queuestat;

    //是否结束线程
    bool m_stop;

    static void* worker(void* arg);
public:
    threadpool(int thread_number = 8,int max_requests = 10000);
    ~threadpool();
    bool append(T* request);
    void run();
};

template<typename T>
threadpool<T>::threadpool(int thread_number,int max_requests):m_thread_number(thread_number),m_max_requests(max_requests),m_stop(false),m_threads(NULL)
{
    if (thread_number<=0 || max_requests <=0)
    {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if(!m_threads){
        throw  std::exception();
    }

    //创建thread_number个线程，并将他们设置为线程脱离
    for(int i=0;i<m_thread_number;++i){
        cout<<"create the "<<i<<"th thread"<<endl;

        //注意C++中，worker必须是一个静态函数,使用this来获得需要的对象的数据
        if(pthread_create(m_threads + i,NULL,worker,this)!=0){
            delete[] m_threads;
            throw std::exception();
        }

        if(pthread_detach(m_threads[i])!=0){
            delete[] m_threads;
            throw std::exception();           
        }

    }
    
}

template<typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request){
    if(m_workqueue.size()>,m_max_requests){
        m_queuelocker.unlock();
        return false;
    }

    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::worker(void* arg){
    threadpool* pool = (threadpool*) arg;
    pool->run();
    return pool; 
}

template<typename T>
void threadpool<T>::run(){
    m_queuestat.wait();
    m_queuelocker.lock();
    T* request = m_workqueue.front();
    m_workqueue.pop_front();
    m_queuelocker.unlock();

    request->process();
}
#endif