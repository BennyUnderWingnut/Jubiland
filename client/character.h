#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "request.pb-c.h"
#include "mylib.h"

#define MAX_LEVEL 60
#define CLASS_TYPES 3

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
} Character;

typedef struct _CharacterNode {
    Character character;
    pthread_mutex_t character_data_mutex;
    struct _CharacterNode *next;
} CharacterNode;

const int EXP_TO_UPGRADE[61];
const int MAX_HP[3][61];
const int MAX_MP[3][61];
const char *CLASS_ICON[3];
const char *CLASS_NAME[3];

Character *init_character(int class);

#endif //SERVER_CHARACTER_H
