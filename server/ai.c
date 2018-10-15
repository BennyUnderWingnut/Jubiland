#include "ai.h"

extern Creature creatures[CREATURE_NUM];

void init_creatures() {
    int i;
    for (i = 0; i < CREATURE_NUM; i++) {
        creatures[i].id = 1;
        creatures[i].category = CREATURE_CATEGORY[i];
        creatures[i].level = CREATURE_LEVEL[i];
        creatures[i].pos_y = creatures[i].born_pos_y = CREATURE_INIT_POSITION[i][0];
        creatures[i].pos_x = creatures[i].born_pos_x = CREATURE_INIT_POSITION[i][1];
        creatures[i].exp = CREATURE_EXP[creatures[i].category][creatures[i].level];
        creatures[i].attack = CREATURE_ATTACK[creatures[i].category][creatures[i].level];
        creatures[i].hp = creatures[i].max_hp = CREATURE_MAX_HP[creatures[i].category][creatures[i].level];
    }
}