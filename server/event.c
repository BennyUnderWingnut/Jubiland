#include "event.h"

MoveEventQueue *move_event_queue_init() {
    MoveEventQueue *mq = malloc(sizeof(*mq));
    mq->head = malloc(sizeof(*mq->head));
    mq->head->next = NULL;
    pthread_mutex_init(&mq->queue_lock, NULL);
    return mq;
}

NewcomerEventQueue *newcomer_event_queue_init() {
    NewcomerEventQueue *nq = malloc(sizeof(*nq));
    nq->head = malloc(sizeof(*nq->head));
    nq->head->next = NULL;
    pthread_mutex_init(&nq->queue_lock, NULL);
    return nq;
}

LogoutEventQueue *logout_event_queue_init() {
    LogoutEventQueue *lq = malloc(sizeof(*lq));
    lq->head = malloc(sizeof(*lq->head));
    lq->head->next = NULL;
    pthread_mutex_init(&lq->queue_lock, NULL);
    return lq;
}

void add_move_event(MoveEventQueue *mq, int id, int pos_y, int pos_x) {
    MoveEvent *me = malloc(sizeof(*me));
    me->id = id;
    me->pos_y = pos_y;
    me->pos_x = pos_x;
    pthread_mutex_lock(&mq->queue_lock);
    me->next = mq->head->next;
    mq->head->next = me;
    mq->event_num++;
    pthread_mutex_unlock(&mq->queue_lock);
}

void add_newcomer_event(NewcomerEventQueue *nq, Character character) {
    NewcomerEvent *ne = malloc(sizeof(*ne));
    ne->class = character.class;
    ne->id = character.id;
    strcpy(ne->nickname, character.nickname);
    ne->pos_y = character.pos_y;
    ne->pos_x = character.pos_x;
    ne->level = character.level;
    ne->exp = character.exp;
    ne->hp = character.hp;
    ne->mp = character.mp;
    pthread_mutex_lock(&nq->queue_lock);
    ne->next = nq->head->next;
    nq->head->next = ne;
    nq->event_num++;
    pthread_mutex_unlock(&nq->queue_lock);
}

void add_logout_event(LogoutEventQueue *lq, int id) {
    LogoutEvent *le = malloc(sizeof(*le));
    le->id = id;
    pthread_mutex_lock(&lq->queue_lock);
    le->next = lq->head->next;
    lq->head->next = le;
    lq->event_num++;
    pthread_mutex_unlock(&lq->queue_lock);
}

MoveEvent *pop_move_events(MoveEventQueue *mq, int *num) {
    pthread_mutex_lock(&mq->queue_lock);
    MoveEvent *me = mq->head->next;
    mq->head->next = NULL;
    *num = mq->event_num;
    mq->event_num = 0;
    pthread_mutex_unlock(&mq->queue_lock);
    return me;
}

NewcomerEvent *pop_newcomer_events(NewcomerEventQueue *nq, int *num) {
    pthread_mutex_lock(&nq->queue_lock);
    NewcomerEvent *me = nq->head->next;
    nq->head->next = NULL;
    *num = nq->event_num;
    nq->event_num = 0;
    pthread_mutex_unlock(&nq->queue_lock);
    return me;
}

LogoutEvent *pop_logout_events(LogoutEventQueue *lq, int *num) {
    pthread_mutex_lock(&lq->queue_lock);
    LogoutEvent *le = lq->head->next;
    lq->head->next = NULL;
    *num = lq->event_num;
    lq->event_num = 0;
    pthread_mutex_unlock(&lq->queue_lock);
    return le;
}