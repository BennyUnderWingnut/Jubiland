#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "request.pb-c.h"

#define MAX_LEVEL 60
#define CLASS_TYPES 3

typedef struct character_status {
    CharacterClass class;
    char nickname[15];
    int id;
    int level;
    int exp;
    int pos_y;
    int pos_x;
    int hp;
    int mp;
} Character;

const int EXP_TO_UPGRADE[61];
const int MAX_HP[3][61];
const int MAX_MP[3][61];
const char *CLASS_ICON[3];
const char *CLASS_NAME[3];

Character *init_character(int class);

#endif //SERVER_CHARACTER_H
