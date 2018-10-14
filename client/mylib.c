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