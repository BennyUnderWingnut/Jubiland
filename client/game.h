#include "mylib.h"
#include "map.h"
#include "login.h"
#include "socklib.h"
#include "locale.h"

#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H
#define SHAPE_EMPTY     "  "
#define SHAPE_GRASS     "🌱"
#define SHAPE_WATER     "🌊"
#define SHAPE_MOUNTAIN  "⛰️"
#define SHAPE_PLAYER    "👦"

int is_move_okay(int y, int x);

void update_graph(void);

void init_curses(void);

void main_loop(void);

void init_world(void);

#endif //CLIENT_GAME_H
