#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include "map.h"
#include "mylib.h"
#include "request.pb-c.h"
#include "response.pb-c.h"

#define CHARACTER_MAX_LEVEL 20
#define CHARACTER_CLASS_TYPES 3

typedef struct character_status {
    CharacterClass class;
    char nickname[16];
    int id;
    int level;
    int exp;
    int pos_y;
    int pos_x;
    int hp;
    int mp;

    struct timeval last_move;
    int move_interval_buffer;
} Character;

const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1];
const int PLAYER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];
const int PLAYER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1];

Character *init_character(int class, char *nickname);

#endif //SERVER_CHARACTER_H
