#ifndef SERVER_ASTAR_H
#define SERVER_ASTAR_H

#include "mylib.h"
#include "map.h"

#define AI_SEARCH_LIMIT 100

int get_next_pos(int start_y, int start_x, int end_y, int end_x, int *next_y, int *next_x);

#endif //SERVER_ASTAR_H
