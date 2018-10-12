#ifndef SERVER_THREADLIB_H
#define SERVER_THREADLIB_H

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <sys/select.h>

#define GET_RANDOM_NUMBER_BETWEEN(low, high) (low+(int)random()%(high-low+1))

int set_ticker(int n_msecs);

void create_detached_thread(void *(*start_routine)(void *), void *arg);


#endif //SERVER_THREADLIB_H
