#include "ai.h"

extern Creature creatures[CREATURE_NUM];
extern pthread_mutex_t ai_move_lock;
extern int running;
extern pthread_mutex_t creature_data_lock[CREATURE_NUM];
int wakeup_count = 0;
extern MoveEventQueue *aiMoveEventQueue;

void init_creatures() {
    int i;
    for (i = 0; i < CREATURE_NUM; i++) {
        creatures[i].id = 1;
        creatures[i].category = CREATURE_CATEGORY[i];
        creatures[i].level = CREATURE_LEVEL[i];
        creatures[i].pos_y = creatures[i].born_pos_y = CREATURE_INIT_POSITION[i][0];
        creatures[i].pos_x = creatures[i].born_pos_x = CREATURE_INIT_POSITION[i][1];
        creatures[i].exp = CREATURE_EXP[creatures[i].category][creatures[i].level];
        creatures[i].ad = CREATURE_AD[creatures[i].category][creatures[i].level];
        creatures[i].hp = creatures[i].max_hp = CREATURE_MAX_HP[creatures[i].category][creatures[i].level];
        pthread_mutex_init(&creature_data_lock[i], NULL);
    }
    pthread_mutex_init(&ai_move_lock, NULL);
}

void *ai_loop() {
    while (running) {
        pthread_mutex_lock(&ai_move_lock);
        wakeup_count = (wakeup_count + 1) % 100000;
        for (int i = 0; i < CREATURE_NUM; i++) {
            if (i % 10 != wakeup_count % 10 || creatures[i].state != CREATURE_STATE__WANDERING) continue;
            pthread_mutex_lock(&creature_data_lock[i]);
            int pos_y = creatures[i].pos_y += CREATURE_WANDER_DIR[(wakeup_count / 10 + i) % 16][0];
            int pos_x = creatures[i].pos_x += CREATURE_WANDER_DIR[(wakeup_count / 10 + i) % 16][1];
            pthread_mutex_unlock(&creature_data_lock[i]);
            add_move_event(aiMoveEventQueue, i, pos_y, pos_x);
        }
    }
    return NULL;
}