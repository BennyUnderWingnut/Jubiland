#ifndef SERVER_CREATURE_H
#define SERVER_CREATURE_H

#define CREATURE_NUM 1000
#define SHAPE_DEAD_CREATURE "💀"
#define SHAPE_ATTACK "💥"

#include "mylib.h"
#include "enums.pb-c.h"

typedef struct _Creature {
    int id;
    CreatureCategory category;
    int level;
    int pos_y;
    int pos_x;
    int hp;
    int max_hp;
    int exp;
    CreatureState state;

    CharacterClass effect_class;
    int effect_skill;
    struct timeval effect_time;
} Creature;

Creature creatures[CREATURE_NUM];
pthread_mutex_t creature_data_lock[CREATURE_NUM];
const char *SHAPE_CREATURES[3][4];
const char *CREATURE_NAME[3][4];
const char *CREATURE_CATEGORY_ICON[3];

#endif //SERVER_CREATURE_H
