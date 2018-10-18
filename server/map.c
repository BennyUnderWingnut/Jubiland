#include "map.h"

void init_map() {
    int i, j, k, l;

    for (i = 0; i < MAP_LINES; i++)
        for (j = 0; j < MAP_COLS; j++)
            map[i][j] = TERRAIN_GRASS;

    for (i = 1; i < MAP_LINES - 1; i += 14)
        for (j = 1; j < MAP_COLS; j += 25)
            map[(i * j + i + j + 13) % MAP_LINES][(j * i + j * 123 + 451) % MAP_COLS] = TERRAIN_ROSE;

    for (i = 1; i < MAP_LINES - 1; i += 13)
        for (j = 1; j < MAP_COLS; j += 14)
            map[(i * j + i + j + 53) % MAP_LINES][(j * i + j * 19 + 12) % MAP_COLS] = TERRAIN_TULIP;

    for (i = 0; i < MAP_LINES; i++) {
        map[i][(i * i * i + 1324 * i + 11983) % MAP_LINES] = TERRAIN_TREE;
        map[i][(i * i * i + 124 * i + 14334) % MAP_LINES] = TERRAIN_TREE;
        map[i][(i * i * i + 1542 * i + 13513) % MAP_LINES] = TERRAIN_TREE;
        map[i][(i * i * i + 721 * i + 8423) % MAP_LINES] = TERRAIN_TREE;
    }

    for (i = 0; i < MAP_LINES; i++)
        map[i][(i * i * i + 285 * i + 1351) % MAP_LINES] = TERRAIN_CAMP;

    /* surrounding mountains */
    for (i = 0; i < MAP_LINES; i++) {
        map[i][0] = TERRAIN_MOUNTAIN;
        map[i][MAP_COLS - 1] = TERRAIN_MOUNTAIN;
    }
    for (j = 1; j < MAP_COLS - 1; j++) {
        map[0][j] = TERRAIN_MOUNTAIN;
        map[MAP_LINES - 1][j] = TERRAIN_MOUNTAIN;
    }

    for (i = 20; i < MAP_LINES; i = i + i % 7 + 60)
        for (j = i % 3; j < MAP_COLS; j = j + j % 6 + i % 5 + 92) {
            for (k = 0; k < i % 10 + 42; k++)
                map[i][j + k] = TERRAIN_WATER;
            for (k = 0; k < i % 10 + j % 24; k++)
                for (l = 0; l < j % 5 + j % 30; l++) {
                    if (k % 5 == 3 && l % 6 == 4) map[i + k - l % 3][j + l - k % 2] = TERRAIN_ISLAND;
                    map[i + k][j + l] = TERRAIN_WATER;
                }
        }

    for (i = 10; i < MAP_LINES; i = i + i % 5 + 80)
        for (j = i % 8; j < MAP_COLS; j = j + j % 7 + i % 4 + 74) {
            for (k = 0; k < j % 9 + 30; k++)
                map[i + k][j] = TERRAIN_WATER;
            for (k = 0; k < j % 9 + j % 20; k++)
                for (l = 0; l < j % 12 + j % 20; l++) {
                    map[i + k][j + l] = TERRAIN_WATER;
                    if (k % 5 == 2 && l % 4 == 2) map[i + k - l % 2][j + l - k % 3] = TERRAIN_ISLAND;
                }
        }
}

int is_move_okay(int y, int x) {
    if (y < 0 || y > MAP_LINES - 1 || x < 0 || x > MAP_COLS - 1) return 0;
    return map[y][x] == TERRAIN_GRASS || map[y][x] == TERRAIN_ROSE || map[y][x] == TERRAIN_TULIP;
}