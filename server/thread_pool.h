#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H

#include <pthread.h>

typedef struct thread_pool ThreadPool;

typedef struct job {
    void *(*callback_function)(void *arg); //线程回调函数
    void *arg; //回调函数参数
    struct job *next;
} Job;

struct thread_pool *thread_pool_init(int thread_num, int queue_max_num);

int thread_pool_add_job(ThreadPool *pool, void *(*callback_function)(void *arg), void *arg);

int thread_pool_destroy(ThreadPool *pool);

#endif //SERVER_THREADPOOL_H
