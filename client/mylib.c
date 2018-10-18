#include "mylib.h"

void exit_game() {
    endwin();
    exit(0);
}

void create_detached_thread(void *(*start_routine)(void *), void *arg) {
    pthread_t t;
    pthread_attr_t attr_detached;
    pthread_attr_init(&attr_detached);
    pthread_attr_setdetachstate(&attr_detached, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr_detached, start_routine, arg);
}

double euclidean_distance(int y1, int x1, int y2, int x2) {
    return sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
};

float time_pass_since(struct timeval t) {
    struct timeval cur;
    gettimeofday(&cur, NULL);
    return cur.tv_sec - t.tv_sec + (float) (cur.tv_usec - t.tv_usec) / 1000000;
};

int set_ticker(long n_msecs) {
    struct itimerval new_timeset;
    long n_sec, n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    new_timeset.it_interval.tv_sec = n_sec;        /* set reload  */
    new_timeset.it_interval.tv_usec = n_usecs;      /* new ticker value */
    new_timeset.it_value.tv_sec = n_sec;      /* store this   */
    new_timeset.it_value.tv_usec = n_usecs;     /* and this     */

    return setitimer(ITIMER_REAL, &new_timeset, NULL);
}