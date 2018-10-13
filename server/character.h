#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "mylib.h"
#include "request.pb-c.h"

#define MAX_LEVEL 60
#define CLASS_TYPES 3

typedef struct character_status {
    unsigned long id;
    char nickname[15];
    CharacterClass class;
    unsigned char level;
    unsigned short exp;
    unsigned short pos_y;
    unsigned short pos_x;
    unsigned hp;
    unsigned mp;
} Character;

const unsigned short EXP_TO_UPGRADE[61];
const unsigned short MAX_HP[3][61];
const unsigned short MAX_MP[3][61];

Character *init_character(int class, char *nickname);

#endif //SERVER_CHARACTER_H
