#ifndef SERVER_EVENT_H
#define SERVER_EVENT_H

#include "mylib.h"
#include "character.h"

typedef struct _MoveEvent {
    int id;
    int pos_x;
    int pos_y;
    struct _MoveEvent *next;
} MoveEvent;

typedef struct _MoveEventQueue {
    int event_num;
    MoveEvent *head;
    pthread_mutex_t queue_lock;
} MoveEventQueue;

typedef struct _NewcomerEvent {
    CharacterClass class;
    char nickname[16];
    int id;
    int level;
    int exp;
    int pos_y;
    int pos_x;
    int hp;
    int mp;
    struct _NewcomerEvent *next;
} NewcomerEvent;

typedef struct _NewcomerEventQueue {
    int event_num;
    NewcomerEvent *head;
    pthread_mutex_t queue_lock;
} NewcomerEventQueue;

MoveEventQueue *move_event_queue_init(void);

NewcomerEventQueue *newcomer_event_queue_init(void);

void add_move_event(MoveEventQueue *mq, int id, int pos_y, int pos_x);

void add_newcomer_event(NewcomerEventQueue *nq, Character character);

MoveEvent *pop_move_events(MoveEventQueue *mq, int *num);

NewcomerEvent *pop_newcomer_events(NewcomerEventQueue *nq, int *num);

#endif //SERVER_EVENT_H
