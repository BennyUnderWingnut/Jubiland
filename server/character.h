#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "mylib.h"
#include "request.pb-c.h"
#include "response.pb-c.h"

#define CHARACTER_MAX_LEVEL 20
#define CHARACTER_CLASS_TYPES 3
#define NUM_SKILLS_PER_CLASS 2

typedef struct character_status {
    CharacterClass class;
    char nickname[16];
    int id;
    int level;
    int exp;
    int pos_y;
    int pos_x;
    int max_hp;
    int hp;
    int max_mp;
    int mp;
    int ad;
    struct timeval last_move;
    struct timeval last_cast[NUM_SKILLS_PER_CLASS];
    int move_interval_buffer;
} Character;

const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_AD[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int CHARACTER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];

const int SKILL_CD[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const float SKILL_RANGE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_TARGET_TYPE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_DAMAGE_RATE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];
const int SKILL_MP_COST[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS];

Character *init_character(int class, char *nickname);

void init_characters(void);

#endif //SERVER_CHARACTER_H
