#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "request.pb-c.h"
#include "mylib.h"

#define CLASS_TYPES 3
#define CHARACTER_MAX_LEVEL 20
#define CHARACTER_CLASS_TYPES 3
const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1];
const int SKILL_CD[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const float SKILL_RANGE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_TARGET_TYPE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_MP_COST[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int CHARACTER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_AD[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const char *SHAPE_CLASSES[CHARACTER_CLASS_TYPES];
const char *CLASS_NAME[CHARACTER_CLASS_TYPES];
const char *SHAPE_SKILLS[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];

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
    int max_hp;
    int max_mp;

    CharacterClass effect_class;
    int effect_skill;
    struct timeval effect_time;
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

float get_remaining_cd(int sk);

CharacterNode *get_character_node_by_id(int id);

#endif //SERVER_CHARACTER_H
