#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#define MAP_LINES 1024
#define MAP_COLS 1024
#define BORN_AREA_X_MIN 1
#define BORN_AREA_X_MAX 8
#define BORN_AREA_Y_MIN 1
#define BORN_AREA_Y_MAX 8

typedef enum _TerrainType {
    TERRAIN_EMPTY,
    TERRAIN_GRASS,
    TERRAIN_WATER,
    TERRAIN_MOUNTAIN,
    TERRAIN_ROSE,
    TERRAIN_TULIP,
    TERRAIN_TREE,
    TERRAIN_ISLAND,
    TERRAIN_CAMP
} TerrainType;

int map[MAP_LINES][MAP_COLS];

int is_move_okay(int y, int x);

void init_map(void);

#endif //SERVER_MAP_H
