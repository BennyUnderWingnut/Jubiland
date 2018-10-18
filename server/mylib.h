#ifndef SERVER_THREADLIB_H
#define SERVER_THREADLIB_H

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <sys/select.h>
#include "enums.pb-c.h"

#define GET_RANDOM_NUMBER_BETWEEN(low, high) (low+(int)random()%(high-low+1))

#define MOVE_INTERVAL_USEC 160000 // 移两次动之间的最小间隔
#define MOVE_INTERVAL_MAX_BUFFER_USEC 160000

int set_ticker(long n_msecs);

void create_detached_thread(void *(*start_routine)(void *), void *arg);

double get_euclidean_distance(int y1, int x1, int y2, int x2);

float time_pass_since(struct timeval t);

#endif //SERVER_THREADLIB_H
