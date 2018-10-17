#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "mylib.h"
#include "character.h"
#import "creature.h"

#define REFRESH_INTERVAL_USEC 10000 // 刷新间隔
#define MOVE_INTERVAL_USEC 160000 // 移动间隔

#define COLOR_PAIR__TEXT_SELECTED 1
#define COLOR_PAIR__TERRAIN_EMPTY 2
#define COLOR_PAIR__TERRAIN_GRASS 3
#define COLOR_PAIR__TERRAIN_WATER 4
#define COLOR_PAIR__TERRAIN_MOUNTAIN 5

#define SHAPE_EMPTY     "  "
#define SHAPE_GRASS     "  "
#define SHAPE_WATER     "🌊"
#define SHAPE_MOUNTAIN  "🗻"
#define SHAPE_CHARACTER    "👦"
#define SHAPE_ME "🤴"

typedef struct _Unit {
    UnitType type;
    Character *character;
    Creature *creature;
} Unit;

Unit selected_targets[SELECT_LOOP - 1];
int selected_number;
Unit target;

Unit select_a_target();

void init_interface();

void print_status_panel();

void print_target_panel();

void print_skill_panel();

#endif //CLIENT_INTERFACE_H
