#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "thread_pool.h"

/*
 thread_pool_function --(queue_not_full)--> thread_pool_add_job
 thread_pool_add_job --(queue_not_empty)--> thread_pool_function
 */

struct thread_pool {
    int thread_num; // number of threads
    int queue_max_num; // allowed maximum number of threads
    struct job *head; // head pointer
    struct job *tail; // tail pointer
    pthread_t *pthreads; // all pthread_ts in this thread pool
    pthread_mutex_t mutex; // mutex semaphore
    pthread_cond_t queue_empty; // condition variable when queue is empty
    pthread_cond_t queue_not_empty; //condition variable when queue is not empty
    pthread_cond_t queue_not_full; // condition value when queue is not full
    int queue_cur_num; // number of jobs in queue
    int queue_close; // whether the thread pool is closed
    int pool_close; // whether the thread pool is closed
};

void *thread_pool_function(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    Job *pjob;
    while (1) {
        pthread_mutex_lock(&(pool->mutex));
        // wait until a job comes
        while ((pool->queue_cur_num == 0) && !pool->pool_close) {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->mutex));
        }
        if (pool->pool_close) {
            pthread_mutex_unlock(&(pool->mutex));
            pthread_exit(NULL);
        }
        pool->queue_cur_num--;
        pjob = pool->head;
        if (pool->queue_cur_num == 0) {
            pool->head = pool->tail = NULL;
        } else {
            pool->head = pjob->next;
        }
        if (pool->queue_cur_num == 0) {
            pthread_cond_signal(&(pool->queue_empty));
        }
        if (pool->queue_cur_num < pool->queue_max_num) {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }
        pthread_mutex_unlock(&(pool->mutex));
        // execute callback function using arguments in arg
        printf("Thread start to handle message\n");
        (*(pjob->callback_function))(pjob->arg);
        free(pjob);
    }
}

int thread_pool_add_job(ThreadPool *pool, void *(*callback_function)(void *arg), void *arg) {
    assert(pool != NULL);
    assert(callback_function != NULL);
    assert(arg != NULL);

    pthread_mutex_lock(&(pool->mutex));
    while ((pool->queue_cur_num == pool->queue_max_num) && !(pool->queue_close || pool->pool_close)) {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->mutex));
    }
    if (pool->queue_close || pool->pool_close) {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    Job *pjob = (Job *) malloc(sizeof(Job));
    if (NULL == pjob) {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }
    pjob->callback_function = callback_function;
    pjob->arg = arg;
    pjob->next = NULL;
    if (pool->head == NULL) {
        pool->head = pool->tail = pjob;
        pthread_cond_broadcast(&(pool->queue_not_empty));
    } else {
        pool->tail->next = pjob;
        pool->tail = pjob;
    }
    pool->queue_cur_num++;
    pthread_mutex_unlock(&(pool->mutex));
    return 0;
}

ThreadPool *thread_pool_init(int thread_num, int queue_max_num) {
    ThreadPool *pool = NULL;
    pool = malloc(sizeof(ThreadPool));
    if (NULL == pool) {
        printf("failed to malloc thread_pool!\n");
        return NULL;;
    }
    pool->thread_num = thread_num;
    pool->queue_max_num = queue_max_num;
    pool->queue_cur_num = 0;
    pool->head = NULL;
    pool->tail = NULL;
    if (pthread_mutex_init(&(pool->mutex), NULL)) {
        printf("failed to init mutex!\n");
        return NULL;;
    }
    if (pthread_cond_init(&(pool->queue_empty), NULL)) {
        printf("failed to init queue_empty!\n");
        return NULL;;
    }
    if (pthread_cond_init(&(pool->queue_not_empty), NULL)) {
        printf("failed to init queue_not_empty!\n");
        return NULL;;
    }
    if (pthread_cond_init(&(pool->queue_not_full), NULL)) {
        printf("failed to init queue_not_full!\n");
        return NULL;;
    }
    pool->pthreads = malloc(sizeof(pthread_t) * thread_num);
    if (NULL == pool->pthreads) {
        printf("failed to malloc pthreads!\n");
        return NULL;;
    }
    pool->queue_close = 0;
    pool->pool_close = 0;
    int i;
    for (i = 0; i < pool->thread_num; ++i) {
        if (pthread_create(&(pool->pthreads[i]), NULL, thread_pool_function, (void *) pool))
            perror("Create thread failed: ");
    }
    return pool;
}

int thread_pool_destroy(ThreadPool *pool) {
    assert(pool != NULL);
    pthread_mutex_lock(&(pool->mutex));
    if (pool->queue_close || pool->pool_close) {
        pthread_mutex_unlock(&(pool->mutex));
        return -1;
    }

    pool->queue_close = 1;
    while (pool->queue_cur_num != 0) {
        // unlock mutex, wait until queue is empty and lock mutex
        pthread_cond_wait(&(pool->queue_empty), &(pool->mutex));
    }

    pool->pool_close = 1;
    pthread_mutex_unlock(&(pool->mutex));
    pthread_cond_broadcast(&(pool->queue_not_empty));
    pthread_cond_broadcast(&(pool->queue_not_full));
    int i;
    // suspends execution of this thread until all other threads terminates
    for (i = 0; i < pool->thread_num; ++i) {
        pthread_join(pool->pthreads[i], NULL);
    }

    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->queue_empty));
    pthread_cond_destroy(&(pool->queue_not_empty));
    pthread_cond_destroy(&(pool->queue_not_full));

    free(pool->pthreads);
    free(pool);
    return 0;
}