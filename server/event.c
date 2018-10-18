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

SkillEventQueue *skill_event_queue_init() {
    SkillEventQueue *sq = malloc(sizeof(*sq));
    sq->head = malloc(sizeof(*sq->head));
    sq->head->next = NULL;
    pthread_mutex_init(&sq->queue_lock, NULL);
    return sq;
};

CreatureStateChangeEventQueue *creature_state_change_event_queue_init(void) {
    CreatureStateChangeEventQueue *cq = malloc(sizeof(*cq));
    cq->head = malloc(sizeof(*cq->head));
    cq->head->next = NULL;
    pthread_mutex_init(&cq->queue_lock, NULL);
    return cq;
};


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

void add_skill_event(SkillEventQueue *sq, UnitType source_type, int source_id, UnitType target_type, int target_id,
                     int skill, int source_mp, int target_hp) {
    SkillEvent *se = malloc(sizeof(*se));
    se->source_type = source_type;
    se->source_id = source_id;
    se->target_type = target_type;
    se->target_id = target_id;
    se->skill = skill;
    se->source_mp = source_mp;
    se->target_hp = target_hp;
    pthread_mutex_lock(&sq->queue_lock);
    se->next = sq->head->next;
    sq->head->next = se;
    sq->event_num++;
    pthread_mutex_unlock(&sq->queue_lock);
};

void add_creature_state_change_event(CreatureStateChangeEventQueue *cq, int id, CreatureState state) {
    CreatureStateChangeEvent *ce = malloc(sizeof(*ce));
    ce->id = id;
    ce->state = state;
    pthread_mutex_lock(&cq->queue_lock);
    ce->next = cq->head->next;
    cq->head->next = ce;
    cq->event_num++;
    pthread_mutex_unlock(&cq->queue_lock);
};


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

SkillEvent *pop_skill_events(SkillEventQueue *sq, int *num) {
    pthread_mutex_lock(&sq->queue_lock);
    SkillEvent *se = sq->head->next;
    sq->head->next = NULL;
    *num = sq->event_num;
    sq->event_num = 0;
    pthread_mutex_unlock(&sq->queue_lock);
    return se;
};

CreatureStateChangeEvent *pop_creature_state_change_events(CreatureStateChangeEventQueue *cq, int *num) {
    pthread_mutex_lock(&cq->queue_lock);
    CreatureStateChangeEvent *ce = cq->head->next;
    cq->head->next = NULL;
    *num = cq->event_num;
    cq->event_num = 0;
    pthread_mutex_unlock(&cq->queue_lock);
    return ce;
};
