#ifndef CLIENT_MAP_H
#define CLIENT_MAP_H

#define MAP_LINES 128
#define MAP_COLS 128

typedef enum _TerrainType {
    TERRAIN_EMPTY, TERRAIN_GRASS, TERRAIN_WATER, TERRAIN_MOUNTAIN
} TerrainType;

int map[MAP_LINES][MAP_COLS];

void init_map(void);

#endif //CLIENT_MAP_H
