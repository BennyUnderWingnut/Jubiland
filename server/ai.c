#include "ai.h"

extern Creature creatures[CREATURE_NUM];
extern pthread_mutex_t ai_move_lock;
extern int running;
extern pthread_mutex_t creature_data_lock[CREATURE_NUM];
extern int wakeup_count;
extern MoveEventQueue *aiMoveEventQueue;
extern CreatureStateChangeEventQueue *creatureStateChangeEventQueue;
extern ConnectionQueue *connectionQueue;
extern SkillEventQueue *skillEventQueue;

void init_creatures() {
    int i, j;
    for (i = 0; i < CREATURE_NUM; i++) {
        creatures[i].id = 1;
        creatures[i].category = CREATURE_CATEGORY[i];
        creatures[i].level = CREATURE_LEVEL[i];
        creatures[i].pos_y = creatures[i].born_pos_y = CREATURE_INIT_POSITION[i][0];
        creatures[i].pos_x = creatures[i].born_pos_x = CREATURE_INIT_POSITION[i][1];
        creatures[i].exp = CREATURE_EXP[creatures[i].category][creatures[i].level];
        creatures[i].ad = CREATURE_AD[creatures[i].category][creatures[i].level];
        creatures[i].hp = creatures[i].max_hp = CREATURE_MAX_HP[creatures[i].category][creatures[i].level];
        creatures[i].state = CREATURE_STATE__WANDERING;
        creatures[i].enter_state_time.tv_sec = 0;
        creatures[i].enter_state_time.tv_usec = 0;
        creatures[i].last_move_time.tv_sec = 0;
        creatures[i].last_move_time.tv_usec = 0;
        creatures[i].num_threats = 0;
        for (j = 0; j < MAX_NUM_THREATS; j++) creatures[i].threats[j] = -1;
        pthread_mutex_init(&creature_data_lock[i], NULL);
    }
    pthread_mutex_init(&ai_move_lock, NULL);
}

void *ai_loop() {
    while (running) {
        pthread_mutex_lock(&ai_move_lock);
        pthread_mutex_lock(&connectionQueue->queue_lock);
        int pos_y, pos_x;
        for (int i = 0; i < CREATURE_NUM; i++) {
            pthread_mutex_lock(&creature_data_lock[i]);
            switch (creatures[i].state) {
                case CREATURE_STATE__WANDERING:
                    if (i % 10 != wakeup_count % 10) break;
                    pos_y = creatures[i].pos_y + CREATURE_WANDER_DIR[(wakeup_count / 10 + i) % 16][0];
                    pos_x = creatures[i].pos_x + CREATURE_WANDER_DIR[(wakeup_count / 10 + i) % 16][1];
                    if (is_move_okay(pos_y, pos_x)) {
                        creatures[i].pos_y = pos_y;
                        creatures[i].pos_x = pos_x;
                    }
                    add_move_event(aiMoveEventQueue, i, pos_y, pos_x);
                    if (creatures[i].hp < creatures[i].max_hp) creatures[i].hp++;
                    break;
                case CREATURE_STATE__COMBAT:
                    if (time_pass_since(creatures[i].enter_state_time) > THREAT_LOSE_SEC) {
                        creatures[i].state = CREATURE_STATE__WANDERING;
                        gettimeofday(&creatures[i].enter_state_time, NULL);
                        add_creature_state_change_event(creatureStateChangeEventQueue, i, CREATURE_STATE__WANDERING);
                        creatures[i].num_threats = 0;
                    } else {
                        double nearest_dist = get_euclidean_distance(0, 0, MAP_LINES - 1, MAP_COLS - 1), dist;
                        Connection *nearest_conn = NULL;
                        int nearest_pos_y = -1, nearest_pos_x = -1, nearest_index = -1;
                        // find nearest threat
                        for (int j = 0; j < (creatures[i].num_threats > MAX_NUM_THREATS ? MAX_NUM_THREATS
                                                                                        : creatures[i].num_threats); j++) {
                            Connection *conn = connectionQueue->head->next;
                            while (conn != NULL) {
                                if (conn->character->id == creatures[i].threats[j]) {
                                    pthread_mutex_lock(&conn->character_data_lock);
                                    dist = get_euclidean_distance(conn->character->pos_y, conn->character->pos_x,
                                                                  creatures[i].pos_y, creatures[i].pos_x);
                                    if (dist < nearest_dist) {
                                        nearest_conn = conn;
                                        nearest_dist = dist;
                                        nearest_pos_y = conn->character->pos_y;
                                        nearest_pos_x = conn->character->pos_x;
                                        nearest_index = j;
                                    }
                                    pthread_mutex_unlock(&conn->character_data_lock);
                                    break;
                                }
                                conn = conn->next;
                            }
                        }
                        if (nearest_conn == NULL) break;
                        // go to nearest threat's position
                        if (time_pass_since(creatures[i].last_move_time) >= CREATURE_MOVE_INTERVAL_SEC &&
                            nearest_dist > 1.5) {
                            int *next_pos_y = malloc(sizeof(int)), *next_pos_x = malloc(sizeof(int));
                            if (get_next_pos(creatures[i].pos_y, creatures[i].pos_x, nearest_pos_y, nearest_pos_x,
                                             next_pos_y, next_pos_x) == 0) {
                                creatures[i].pos_y = *next_pos_y;
                                creatures[i].pos_x = *next_pos_x;
                                add_move_event(aiMoveEventQueue, i, *next_pos_y, *next_pos_x);
                                gettimeofday(&creatures[i].last_move_time, NULL);
                            }
                            free(next_pos_y);
                            free(next_pos_x);
                        }
                        // attack nearest threat
                        if (time_pass_since(creatures[i].last_attack_time) >= CREATURE_ATTACK_INTERVAL_SEC &&
                            nearest_dist <= CREATURE_ATTACK_RANGE) {
                            pthread_mutex_lock(&nearest_conn->character_data_lock);
                            nearest_conn->character->hp -= creatures[i].ad;
                            if (nearest_conn->character->hp <= 0) {
                                nearest_conn->character->hp = 0;
                                creatures[i].threats[nearest_index] = creatures[i].threats[
                                        (creatures[i].num_threats - 1) % MAX_NUM_THREATS];
                                creatures[i].threats[
                                        (creatures[i].num_threats - 1) % MAX_NUM_THREATS] = -1;
                                creatures[i].num_threats--;
                                if (creatures[i].num_threats == 0) {
                                    creatures[i].state = CREATURE_STATE__WANDERING;
                                    gettimeofday(&creatures[i].enter_state_time, NULL);
                                    add_creature_state_change_event(creatureStateChangeEventQueue, i,
                                                                    CREATURE_STATE__WANDERING);
                                }
                            }
                            add_skill_event(skillEventQueue, UNIT_TYPE__CREATURE, i, UNIT_TYPE__CHARACTER,
                                            nearest_conn->character->id, 0, 0, nearest_conn->character->hp);
                            pthread_mutex_unlock(&nearest_conn->character_data_lock);
                            gettimeofday(&creatures[i].last_attack_time, NULL);
                        }
                    }
                    break;
                case CREATURE_STATE__DEAD:
                    if (time_pass_since(creatures[i].enter_state_time) <
                        CREATURE_REFRESH_INTERVAL_SEC[creatures[i].category])
                        break;
                    creatures[i].hp = creatures[i].max_hp;
                    creatures[i].state = CREATURE_STATE__WANDERING;
                    gettimeofday(&creatures[i].enter_state_time, NULL);
                    add_creature_state_change_event(creatureStateChangeEventQueue, i, CREATURE_STATE__WANDERING);
                    creatures[i].num_threats = 0;
                    break;
                default:;
            }
            pthread_mutex_unlock(&creature_data_lock[i]);
        }
        pthread_mutex_unlock(&connectionQueue->queue_lock);
    }
    return NULL;
}