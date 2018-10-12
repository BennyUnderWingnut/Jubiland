#include "map.h"

extern int map[MAP_LINES][MAP_COLS];

void init_map() {
    int i, j;

    for (i = 0; i < MAP_LINES; i++)
        for (j = 0; j < MAP_COLS; j++)
            map[i][j] = TERRAIN_GRASS;

    /* surrounding mountains */
    for (i = 0; i < MAP_LINES; i++) {
        map[i][0] = TERRAIN_MOUNTAIN;
        map[i][MAP_COLS - 1] = TERRAIN_MOUNTAIN;
    }
    for (j = 1; j < MAP_COLS - 1; j++) {
        map[0][j] = TERRAIN_MOUNTAIN;
        map[MAP_LINES - 1][j] = TERRAIN_MOUNTAIN;
    }

    for (i = 4; i < MAP_LINES; i += 10) {
        for (j = 1; j < 10; j++)
            map[i][j] = TERRAIN_WATER;
    }
}