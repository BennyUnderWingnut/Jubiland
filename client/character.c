#include "character.h"

const char *CLASS_ICON[3] = {"🛡 ", "🔥", "📖"};
const char *CLASS_NAME[3] = {"Warrior", "Mage", "Priest"};
const char *SKILL_ICON[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{"🗡️", "🗯"},
                                                                       {"👊",  "☄️"},
                                                                       {"👊",  "💖"}};

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

const int EXP_TO_UPGRADE[CHARACTER_MAX_LEVEL + 1] = {
        0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900,
        2147483647};

const int SKILL_CD[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{1, 5},
                                                                   {1, 2},
                                                                   {1, 2}
};

const int SKILL_RANGE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{2, 2},
                                                                      {2, 10},
                                                                      {2, 10}};

const int SKILL_TARGET_TYPE[CHARACTER_CLASS_TYPES][NUM_SKILLS_PER_CLASS] = {{UNIT_TYPE__CREATURE, UNIT_TYPE__CREATURE},
                                                                            {UNIT_TYPE__CREATURE, UNIT_TYPE__CREATURE},
                                                                            {UNIT_TYPE__CREATURE, UNIT_TYPE__CHARACTER}};

float get_remaining_cd(int i) {
    struct timeval cur;
    gettimeofday(&cur, NULL);
    float remaining = SKILL_CD[me->character.class][i] -
                      cur.tv_sec + last_cast_time[i].tv_sec -
                      (float) (cur.tv_usec - last_cast_time[i].tv_usec) / 1000000;
    return remaining > 0 ? remaining : 0;
}