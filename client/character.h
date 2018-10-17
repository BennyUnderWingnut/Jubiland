#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "request.pb-c.h"
#include "mylib.h"

#define CLASS_TYPES 3
#define CHARACTER_MAX_LEVEL 20
#define CHARACTER_CLASS_TYPES 3
const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const char *CLASS_ICON[CHARACTER_CLASS_TYPES];
const char *CLASS_NAME[CHARACTER_CLASS_TYPES];
const char *SKILL_ICON[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_CD[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_RANGE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_TARGET_TYPE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];

typedef struct _Character {
    CharacterClass class;
    char nickname[15];
    int id;
    int level;
    int exp;
    int pos_y;
    int pos_x;
    int hp;
    int mp;
    int ad;
} Character;

typedef struct _CharacterNode {
    Character character;
    pthread_mutex_t character_data_lock;
    pthread_mutex_t next_lock;
    struct _CharacterNode *next;
} CharacterNode;

CharacterNode *ch_list_head, *me;

struct timeval last_cast_time[NUM_SKILLS_PER_CLASS];

Character *init_character(int class);

float get_remaining_cd(int i);

#endif //SERVER_CHARACTER_H
