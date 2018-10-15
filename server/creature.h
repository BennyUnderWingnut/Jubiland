#ifndef SERVER_CREATURE_H
#define SERVER_CREATURE_H

#define CREATURE_MAX_LEVEL 20
#define CREATURE_CATEGORIES_NUM 3
#define CREATURE_NUM 1000

#include "mylib.h"
#include "enums.pb-c.h"

typedef struct _Creature {
    int id;
    CreatureCategory category;
    int attack;
    int level;
    int exp; // exp the player killing it gets
    int pos_y;
    int pos_x;
    int born_pos_y;
    int born_pos_x;
    int hp;
    int max_hp;
} Creature;

const int CREATURE_MAX_HP[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_ATTACK[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_EXP[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_CATEGORY[CREATURE_NUM];

const int CREATURE_LEVEL[CREATURE_NUM];

const int CREATURE_INIT_POSITION[CREATURE_NUM][2];

const int CREATURE_REFRESH_INTERVAL_SEC[CREATURE_CATEGORIES_NUM];

#endif //SERVER_CREATURE_H
