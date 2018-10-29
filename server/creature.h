#ifndef SERVER_CREATURE_H
#define SERVER_CREATURE_H

#define CREATURE_MAX_LEVEL 20
#define CREATURE_CATEGORIES_NUM 3
#define CREATURE_NUM 1000
#define CREATURE_ATTACK_RANGE 1.5
#define CREATURE_ATTACK_INTERVAL_SEC 1
#define MAX_NUM_THREATS 5

#include "mylib.h"
#include "enums.pb-c.h"

typedef struct _Creature {
    int id;
    CreatureCategory category;
    int ad;
    int level;
    int exp; // exp the player killing it gets
    int pos_y;
    int pos_x;
    int born_pos_y;
    int born_pos_x;
    int hp;
    int max_hp;
    CreatureState state;
    struct timeval enter_state_time;
    struct timeval last_move_time;
    struct timeval last_attack_time;
    int num_threats;
    int threats[MAX_NUM_THREATS];
} Creature;

const int CREATURE_MAX_HP[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_AD[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_EXP[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

const int CREATURE_CATEGORY[CREATURE_NUM];

const int CREATURE_LEVEL[CREATURE_NUM];

const int CREATURE_INIT_POSITION[CREATURE_NUM][2];

const int CREATURE_REFRESH_INTERVAL_SEC[CREATURE_CATEGORIES_NUM];

const int CREATURE_WANDER_DIR[16][2];

const int CREATURE_SPEED[CREATURE_CATEGORIES_NUM][CREATURE_MAX_LEVEL + 1];

#endif //SERVER_CREATURE_H
