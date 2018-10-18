#include "character.h"

const int CHARACTER_MAX_HP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 200, 240, 280, 320, 360, 400, 440, 480, 520, 560, 600, 640, 680, 720, 760, 800, 840, 880, 920, 960},
        {0, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480},
        {0, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480}
};

const int CHARACTER_MAX_MP[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
        {0, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480},
        {0, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480}
};

const int CHARACTER_AD[CHARACTER_CLASS_TYPES][CHARACTER_MAX_LEVEL + 1] = {
        {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200},
        {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200},
        {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}
};

const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1] = {
        0, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
        2147483647};

const int SKILL_CD[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{1, 5},
                                                                   {1, 2},
                                                                   {1, 2}
};

const float SKILL_RANGE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{1.5, 1.5},
                                                                        {1.5, 10},
                                                                        {1.5, 10}};

const int SKILL_TARGET_TYPE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{UNIT_TYPE__CREATURE, UNIT_TYPE__CREATURE},
                                                                            {UNIT_TYPE__CREATURE, UNIT_TYPE__CREATURE},
                                                                            {UNIT_TYPE__CREATURE, UNIT_TYPE__CHARACTER}};

const int SKILL_DAMAGE_RATE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{2, 5},
                                                                            {1, 5},
                                                                            {1, -5}};

const int SKILL_MP_COST[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{0, 10},
                                                                        {0, 10},
                                                                        {0, 10}};

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
    character->hp = character->max_hp = CHARACTER_MAX_HP[class][1];
    character->mp = character->max_mp = CHARACTER_MAX_MP[class][1];
    character->ad = CHARACTER_AD[class][1];
    character->pos_x = GET_RANDOM_NUMBER_BETWEEN(BORN_AREA_X_MIN, BORN_AREA_X_MAX);
    character->pos_y = GET_RANDOM_NUMBER_BETWEEN(BORN_AREA_Y_MIN, BORN_AREA_Y_MAX);
    gettimeofday(&character->last_move, NULL);
    for (int i = 0; i < NUM_SKILLS_PER_CLASS; i++) {
        character->last_cast[i].tv_sec = 0;
        character->last_cast[i].tv_usec = 0;
    }
    character->move_interval_buffer = 0;
    return character;
}

void init_characters(void) {
    pthread_mutex_init(&last_character_id_lock, NULL);

}