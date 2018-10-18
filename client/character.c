#include "character.h"

const char *SHAPE_CLASSES[3] = {"🛡 ", "🔥", "📖"};
const char *CLASS_NAME[3] = {"Warrior", "Mage", "Priest"};
const char *SHAPE_SKILLS[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{"🗡️", "🗯"},
                                                                         {"👊",  "☄️"},
                                                                         {"👊",  "💖"}};

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

const int SKILL_MP_COST[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{0, 10},
                                                                        {0, 10},
                                                                        {0, 10}};

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

float get_remaining_cd(int sk) {
    struct timeval cur;
    gettimeofday(&cur, NULL);
    float remaining = SKILL_CD[me->character.class][sk] -
                      cur.tv_sec + last_cast_time[sk].tv_sec -
                      (float) (cur.tv_usec - last_cast_time[sk].tv_usec) / 1000000;
    return remaining > 0 ? remaining : 0;
}

CharacterNode *get_character_node_by_id(int id) {
    CharacterNode *node = ch_list_head->next;
    while (node != NULL) {
        if (node->character.id == id) return node;
        node = node->next;
    }
    return NULL;
}
