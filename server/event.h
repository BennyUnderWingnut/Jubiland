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

typedef struct _LogoutEvent {
    int id;
    struct _LogoutEvent *next;
} LogoutEvent;

typedef struct _LogoutEventQueue {
    int event_num;
    LogoutEvent *head;
    pthread_mutex_t queue_lock;
} LogoutEventQueue;

typedef struct _SkillEvent {
    UnitType source_type;
    int source_id;
    UnitType target_type;
    int target_id;
    int skill;
    int source_mp;
    int target_hp;
    struct _SkillEvent *next;
} SkillEvent;

typedef struct _SkillEventQueue {
    int event_num;
    SkillEvent *head;
    pthread_mutex_t queue_lock;
} SkillEventQueue;

typedef struct _CreatureStateChangeEvent {
    int id;
    CreatureState state;
    struct _CreatureStateChangeEvent *next;
} CreatureStateChangeEvent;

typedef struct _StateChangeEventQueue {
    int event_num;
    CreatureStateChangeEvent *head;
    pthread_mutex_t queue_lock;
} CreatureStateChangeEventQueue;

MoveEventQueue *move_event_queue_init(void);

NewcomerEventQueue *newcomer_event_queue_init(void);

LogoutEventQueue *logout_event_queue_init(void);

SkillEventQueue *skill_event_queue_init(void);

CreatureStateChangeEventQueue *creature_state_change_event_queue_init(void);

void add_move_event(MoveEventQueue *mq, int id, int pos_y, int pos_x);

void add_newcomer_event(NewcomerEventQueue *nq, Character character);

void add_logout_event(LogoutEventQueue *lq, int id);

void add_skill_event(SkillEventQueue *sq, UnitType source_type, int source_id, UnitType target_type, int target_id,
                     int skill, int source_mp, int target_hp);

void add_creature_state_change_event(CreatureStateChangeEventQueue *cq, int id, CreatureState state);

MoveEvent *pop_move_events(MoveEventQueue *mq, int *num);

NewcomerEvent *pop_newcomer_events(NewcomerEventQueue *nq, int *num);

LogoutEvent *pop_logout_events(LogoutEventQueue *lq, int *num);

SkillEvent *pop_skill_events(SkillEventQueue *sq, int *num);

CreatureStateChangeEvent *pop_creature_state_change_events(CreatureStateChangeEventQueue *cq, int *num);

#endif //SERVER_EVENT_H
