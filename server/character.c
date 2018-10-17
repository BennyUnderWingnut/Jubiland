#include "character.h"

const int CHARACTER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 100, 200, 300, 400, 500,  600,  700,  800,  900,  1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000},
        {0, 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 3800, 4000},
        {0, 100, 200, 300, 400, 500,  600,  700,  800,  900,  1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000}
};

const int CHARACTER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000},
        {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100},
        {0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000}
};

const int CHARACTER_AD[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 10, 20, 30, 40, 50,  60,  70,  80,  90,  100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200},
        {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400},
        {0, 10, 20, 30, 40, 50,  60,  70,  80,  90,  100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}
};

const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1] = {
        0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
        2147483647};

int last_character_id = 1;
pthread_mutex_t last_character_id_lock;

Character *init_character(int class, char *nickname) {
    if (strlen(nickname) > 15)
        return NULL;
    Character *character = malloc(sizeof(Character));
    pthread_mutex_lock(&last_character_id_lock);
    character->id = last_character_id++;
    pthread_mutex_unlock(&last_character_id_lock);
    strcpy(character->nickname, nickname);
    character->class = class;
    character->level = 1;
    character->exp = 0;
    character->hp = CHARACTER_MAX_HP[class][1];
    character->mp = CHARACTER_MAX_MP[class][1];
    character->ad = CHARACTER_AD[class][1];
    character->pos_x = GET_RANDOM_NUMBER_BETWEEN(BORN_AREA_X_MIN, BORN_AREA_X_MAX);
    character->pos_y = GET_RANDOM_NUMBER_BETWEEN(BORN_AREA_Y_MIN, BORN_AREA_Y_MAX);
    gettimeofday(&character->last_move, NULL);
    for (int i = 0; i < NUM_SKILLS_PER_CLASS; i++) {
        character->last_cast->tv_sec = 0;
        character->last_cast->tv_usec = 0;
    }
    character->move_interval_buffer = 0;
    return character;
}

void init_characters(void) {
    pthread_mutex_init(&last_character_id_lock, NULL);

}