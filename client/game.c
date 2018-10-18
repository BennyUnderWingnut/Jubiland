#include "game.h"

const char *SHAPES[] = {SHAPE_EMPTY, SHAPE_GRASS, SHAPE_WATER, SHAPE_MOUNTAIN, SHAPE_ROSE, SHAPE_TULIP, SHAPE_TREE,
                        SHAPE_ISLAND, SHAPE_CAMP};
int running = 1;

int is_move_okay(int y, int x);

int is_cast_okay(int skill);

void update_graph(void);

void wait_until_movable(struct timeval *last_move, struct timeval *this_move);

void main_loop() {
    clear();
    curs_set(0);/* hide cursor */
    int ch, rv;
    fd_set set;
    struct timeval timeout, last_move, this_move;
    gettimeofday(&last_move, NULL);
    while (running) {
        update_graph();
        refresh();
        FD_ZERO(&set);
        FD_SET(0, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = REFRESH_INTERVAL_USEC;
        rv = select(1, &set, NULL, NULL, &timeout);
        if (rv == 0 || rv == -1) continue;
        ch = getch();
        switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
                if (is_move_okay(me->character.pos_y - 1, me->character.pos_x)) {
                    wait_until_movable(&last_move, &this_move);
                    me->character.pos_y--;
                    sync_move();
                }
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                if (is_move_okay(me->character.pos_y + 1, me->character.pos_x)) {
                    wait_until_movable(&last_move, &this_move);
                    me->character.pos_y++;
                    sync_move();
                }
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                if (is_move_okay(me->character.pos_y, me->character.pos_x - 1)) {
                    wait_until_movable(&last_move, &this_move);
                    me->character.pos_x--;
                    sync_move();
                }
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                if (is_move_okay(me->character.pos_y, me->character.pos_x + 1)) {
                    wait_until_movable(&last_move, &this_move);
                    me->character.pos_x++;
                    sync_move();
                }
                break;
            case 9:
                target = select_a_target();
                break;
            case '1':
                if (is_cast_okay(0))
                    sync_skill(0);
                break;
            case '2':
                if (is_cast_okay(1))
                    sync_skill(1);
                break;
            case 27:
            case 'q':
            case 'Q':
                end_game();
                return;
            default:;
        }
    }
}

void update_graph(void) {
    int i, j, map_y, map_x, screen_y, screen_x, effect_skill, hp;
    struct timeval effect_time;
    CharacterClass effect_class;
    for (i = 1; i < LINES - 1; i++) {
        for (j = 0; j < COLS - 1; j += 2) {
            map_y = me->character.pos_y + i - (LINES - 1) / 2;
            map_x = me->character.pos_x + j / 2 - (COLS - 1) / 4;
            if (map_y < 0 || map_y >= MAP_LINES || map_x < 0 || map_x >= MAP_COLS) {
                attron(COLOR_PAIR(TERRAIN_EMPTY));
                mvaddstr(i, j, SHAPE_EMPTY);
                attroff(COLOR_PAIR(TERRAIN_EMPTY));
            } else {
                attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
                mvaddstr(i, j, SHAPES[map[map_y][map_x]]);
                attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            }
        }
    }
    CharacterNode *node = ch_list_head, *prev;
    while (node->next != NULL) {
        pthread_mutex_lock(&node->next_lock);
        prev = node;
        node = node->next;
        pthread_mutex_lock(&node->character_data_lock);
        pthread_mutex_unlock(&prev->next_lock);
        map_y = node->character.pos_y;
        map_x = node->character.pos_x;
        effect_class = node->character.effect_class;
        effect_skill = node->character.effect_skill;
        effect_time = node->character.effect_time;
        hp = node->character.hp;
        pthread_mutex_unlock(&node->character_data_lock);
        screen_y = map_y - me->character.pos_y + (LINES - 1) / 2;
        screen_x = 2 * (map_x - me->character.pos_x + (COLS - 1) / 4);
        if (screen_y > 0 && screen_y < LINES - 1 && screen_x > 1 && screen_x < COLS - 1) {
            attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            if (hp <= 0)
                mvaddstr(screen_y, screen_x, SHAPE_DEAD_CHARACTER);
            else if (time_pass_since(effect_time) < EFFECT_SHOW_TIME_SEC)
                mvaddstr(screen_y, screen_x,
                         effect_class == -1 ? SHAPE_ATTACK : SHAPE_SKILLS[effect_class][effect_skill]);
            else
                mvaddstr(screen_y, screen_x, SHAPE_CHARACTER);
            attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
        }
    }
    for (i = 0; i < CREATURE_NUM; i++) {
        pthread_mutex_lock(&creature_data_lock[i]);
        map_y = creatures[i].pos_y;
        map_x = creatures[i].pos_x;
        effect_class = creatures[i].effect_class;
        effect_skill = creatures[i].effect_skill;
        effect_time = creatures[i].effect_time;
        pthread_mutex_unlock(&creature_data_lock[i]);
        screen_y = map_y - me->character.pos_y + (LINES - 1) / 2;
        screen_x = 2 * (map_x - me->character.pos_x + (COLS - 1) / 4);
        if (screen_y > 0 && screen_y < LINES - 1 && screen_x > 0 && screen_x < COLS - 1) {
            attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            if (creatures[i].state == CREATURE_STATE__DEAD) mvaddstr(screen_y, screen_x, SHAPE_DEAD_CREATURE);
            else if (time_pass_since(effect_time) < EFFECT_SHOW_TIME_SEC)
                mvaddstr(screen_y, screen_x, SHAPE_SKILLS[effect_class][effect_skill]);
            else
                mvaddstr(screen_y, screen_x, SHAPE_CREATURES[creatures[i].category][(creatures[i].level - 1) / 5]);
            attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
        }
    }
    if (target.type == UNIT_TYPE__CHARACTER) {
        map_y = target.character->pos_y;
        map_x = target.character->pos_x;
        effect_class = target.character->effect_class;
        effect_skill = target.character->effect_skill;
        effect_time = target.character->effect_time;
        screen_y = map_y - me->character.pos_y + (LINES - 1) / 2;
        screen_x = 2 * (map_x - me->character.pos_x + (COLS - 1) / 4);
        hp = target.character->hp;
        if (screen_y > 0 && screen_y < LINES - 1 && screen_x > 0 && screen_x < COLS - 1) {
            attron(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
            if (hp <= 0)
                mvaddstr(screen_y, screen_x, SHAPE_DEAD_CHARACTER);
            else if (time_pass_since(effect_time) < EFFECT_SHOW_TIME_SEC)
                mvaddstr(screen_y, screen_x,
                         effect_class == -1 ? SHAPE_ATTACK : SHAPE_SKILLS[effect_class][effect_skill]);
            else
                mvaddstr(screen_y, screen_x, SHAPE_CHARACTER);
            attroff(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
        }
    } else if (target.type == UNIT_TYPE__CREATURE) {
        map_y = target.creature->pos_y;
        map_x = target.creature->pos_x;
        screen_y = map_y - me->character.pos_y + (LINES - 1) / 2;
        screen_x = 2 * (map_x - me->character.pos_x + (COLS - 1) / 4);
        effect_class = target.creature->effect_class;
        effect_skill = target.creature->effect_skill;
        effect_time = target.creature->effect_time;
        if (screen_y > 0 && screen_y < LINES - 1 && screen_x > 0 && screen_x < COLS - 1) {
            attron(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
            if (target.creature->state == CREATURE_STATE__DEAD) mvaddstr(screen_y, screen_x, SHAPE_DEAD_CREATURE);
            else if (time_pass_since(effect_time) < EFFECT_SHOW_TIME_SEC)
                mvaddstr(screen_y, screen_x, SHAPE_SKILLS[effect_class][effect_skill]);
            else
                mvaddstr(screen_y, screen_x,
                         SHAPE_CREATURES[target.creature->category][(target.creature->level - 1) / 5]);
            attroff(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
        }
    }
    attron(COLOR_PAIR(get_terrain_color_pair(map[(LINES - 1) / 2][(COLS - 1) / 4 * 2])));
    if (me->character.hp <= 0);
    else if (time_pass_since(me->character.effect_time) < EFFECT_SHOW_TIME_SEC) {
        pthread_mutex_lock(&me->character_data_lock);
        mvaddstr((LINES - 1) / 2, (COLS - 1) / 4 * 2,
                 me->character.effect_class == -1 ? SHAPE_ATTACK
                                                  : SHAPE_SKILLS[me->character.effect_class][me->character.effect_skill]);
        pthread_mutex_unlock(&me->character_data_lock);
    } else
        mvaddstr((LINES - 1) / 2, (COLS - 1) / 4 * 2, SHAPE_ME);
    attroff(COLOR_PAIR(get_terrain_color_pair(map[(LINES - 1) / 2][(COLS - 1) / 4 * 2])));

    print_status_panel();
    print_target_panel();
    print_skill_panel();
}

void *listen_events(void *sockptr) {
    int sock = *(int *) sockptr;
    fd_set set;
    Response *resp;
    MoveMessage *mm;
    CharacterMessage *cm;
    LogoutMessage *lm;
    SkillMessage *sm;
    CreatureStateChangeMessage *csm;
    int rv, i, repeated;
    CharacterNode *ch, *to_free;
    while (running) {
        FD_ZERO(&set);
        FD_SET(sock, &set);
        rv = select(sock + 1, &set, NULL, NULL, 0);
        if (rv == -1) end_game();
        resp = get_response(sock);
        if (resp == NULL) end_game();
        else if (resp->type == RESPONSE__TYPE__EVENTS) {
            if (resp->events == NULL) end_game();
            for (i = 0; i < resp->events->n_moves; i++) {
                mm = resp->events->moves[i];
                for (ch = ch_list_head->next; ch != NULL; ch = ch->next)
                    if (ch->character.id == mm->id) {
                        pthread_mutex_lock(&ch->character_data_lock);
                        ch->character.pos_y = mm->pos_y;
                        ch->character.pos_x = mm->pos_x;
                        if (map[ch->character.pos_y][ch->character.pos_x] == TERRAIN_ROSE)
                            ch->character.hp = ch->character.hp + 5 > ch->character.max_hp ? ch->character.max_hp :
                                               ch->character.hp + 5;
                        if (map[ch->character.pos_y][ch->character.pos_x] == TERRAIN_TULIP)
                            ch->character.mp = ch->character.mp + 5 > ch->character.max_mp ? ch->character.max_mp :
                                               ch->character.mp + 5;
                        pthread_mutex_unlock(&ch->character_data_lock);
                        break;
                    }
            }
            for (i = 0; i < resp->events->n_aimoves; i++) {
                mm = resp->events->aimoves[i];
                pthread_mutex_lock(&creature_data_lock[i]);
                creatures[mm->id].pos_y = mm->pos_y;
                creatures[mm->id].pos_x = mm->pos_x;
                if (creatures[mm->id].state == CREATURE_STATE__WANDERING &&
                    creatures[mm->id].hp < creatures[mm->id].max_hp)
                    creatures[mm->id].hp++;
                pthread_mutex_unlock(&creature_data_lock[i]);
            }
            for (i = 0; i < resp->events->n_newcomers; i++) {
                cm = resp->events->newcomers[i];
                repeated = 0;
                for (ch = ch_list_head->next; ch != NULL; ch = ch->next)
                    if (ch->character.id == cm->id) {
                        repeated = 1;
                        break;
                    }
                if (repeated) continue;
                CharacterNode *node = malloc(sizeof(*node));
                node->next = ch_list_head->next;
                node->character.id = cm->id;
                node->character.hp = cm->hp;
                node->character.max_hp = cm->hp;
                node->character.mp = cm->mp;
                node->character.max_mp = cm->max_mp;
                node->character.level = cm->level;
                node->character.exp = cm->exp;
                strcpy(node->character.nickname, cm->nickname);
                node->character.class = cm->class_;
                node->character.ad = cm->ad;
                node->character.pos_y = cm->pos_y;
                node->character.pos_x = cm->pos_x;
                node->character.effect_time.tv_usec = 0;
                node->character.effect_time.tv_sec = 0;
                ch_list_head->next = node;
            }
            for (i = 0; i < resp->events->n_logouts; i++) {
                lm = resp->events->logouts[i];
                for (ch = ch_list_head; ch->next != NULL; ch = ch->next)
                    if (ch->next->character.id == lm->id) {
                        pthread_mutex_lock(&ch->next_lock);
                        to_free = ch->next;
                        ch->next = ch->next->next;
                        pthread_mutex_unlock(&ch->next_lock);
                        if (target.type == UNIT_TYPE__CHARACTER &&
                            target.character->id == to_free->character.id)
                            target.type = -1;
                        free(to_free);
                        break;
                    }
            }
            for (i = 0; i < resp->events->n_skills; i++) {
                sm = resp->events->skills[i];
                CharacterNode *source_character = NULL;
                if (sm->source_type == UNIT_TYPE__CHARACTER) {
                    source_character = get_character_node_by_id(sm->source_id);
                    if (me->character.id == sm->source_id) gettimeofday(&last_cast_time[sm->skill], NULL);
                    for (ch = ch_list_head->next; ch != NULL; ch = ch->next)
                        if (ch->character.id == sm->source_id) {
                            pthread_mutex_lock(&ch->character_data_lock);
                            ch->character.mp = sm->source_mp;
                            pthread_mutex_unlock(&ch->character_data_lock);
                            break;
                        }
                }
                if (sm->target_type == UNIT_TYPE__CHARACTER) {
                    for (ch = ch_list_head->next; ch != NULL; ch = ch->next)
                        if (ch->character.id == sm->target_id) {
                            pthread_mutex_lock(&ch->character_data_lock);
                            ch->character.hp = sm->target_hp;
                            ch->character.effect_class =
                                    source_character == NULL ? -1 : source_character->character.class;
                            ch->character.effect_skill = sm->skill;
                            gettimeofday(&ch->character.effect_time, NULL);
                            pthread_mutex_unlock(&ch->character_data_lock);
                            break;
                        }
                } else {
                    pthread_mutex_lock(&creature_data_lock[sm->target_id]);
                    creatures[sm->target_id].hp = sm->target_hp;
                    creatures[sm->target_id].effect_class =
                            source_character == NULL ? -1 : source_character->character.class;
                    creatures[sm->target_id].effect_skill = sm->skill;
                    gettimeofday(&creatures[sm->target_id].effect_time, NULL);
                    if (creatures[sm->target_id].hp <= 0) {
                        creatures[sm->target_id].state = CREATURE_STATE__DEAD;
                        creatures[sm->target_id].hp = 0;
                        pthread_mutex_lock(&source_character->character_data_lock);
                        source_character->character.exp += creatures[sm->target_id].exp;
                        while (source_character->character.exp >= EXP_TO_UPGRADE[source_character->character.level]) {
                            source_character->character.exp -= EXP_TO_UPGRADE[source_character->character.level];
                            source_character->character.level++;
                            source_character->character.hp = source_character->character.max_hp = CHARACTER_MAX_HP[source_character->character.class][source_character->character.level];
                            source_character->character.mp = source_character->character.max_mp = CHARACTER_MAX_MP[source_character->character.class][source_character->character.level];
                            source_character->character.ad = CHARACTER_AD[source_character->character.class][source_character->character.level];
                        }
                        pthread_mutex_unlock(&source_character->character_data_lock);
                    } else creatures[sm->target_id].state = CREATURE_STATE__COMBAT;
                    pthread_mutex_unlock(&creature_data_lock[sm->target_id]);
                }
            }
            for (i = 0; i < resp->events->n_cschanges; i++) {
                csm = resp->events->cschanges[i];
                pthread_mutex_lock(&creature_data_lock[csm->id]);
                if (creatures[csm->id].state == CREATURE_STATE__DEAD)
                    creatures[csm->id].hp = creatures[csm->id].max_hp;
                creatures[csm->id].state = csm->state;
                pthread_mutex_unlock(&creature_data_lock[csm->id]);
            }
        } else if (resp->type == RESPONSE__TYPE__RESET_KEY) {
            if (resp->resetkey == NULL) end_game();
            key = resp->resetkey->key;
        }
        response__free_unpacked(resp, NULL);
    }
    return NULL;
}

int is_move_okay(int y, int x) {
    if (me->character.hp <= 0 || y < 0 || y > MAP_LINES - 1 || x < 0 || x > MAP_COLS - 1) return 0;
    return map[y][x] == TERRAIN_GRASS || map[y][x] == TERRAIN_ROSE || map[y][x] == TERRAIN_TULIP;
}

void wait_until_movable(struct timeval *last_move, struct timeval *this_move) {
    gettimeofday(this_move, NULL);
    int delta = (int) ((this_move->tv_sec - last_move->tv_sec) * 1000000 +
                       this_move->tv_usec - last_move->tv_usec);
    if (delta < MOVE_INTERVAL_USEC)
        usleep((useconds_t) (MOVE_INTERVAL_USEC - delta));
    *last_move = *this_move;
}

void end_game() {
    running = 0;
    close(sock);
    exit_game();
}

int is_cast_okay(int skill) {
    if (me->character.hp <= 0 || me->character.mp < SKILL_MP_COST[me->character.class][skill]) return 0;
    if ((target.type == UNIT_TYPE__CREATURE &&
         SKILL_TARGET_TYPE[me->character.class][skill] == UNIT_TYPE__CREATURE &&
         euclidean_distance(target.creature->pos_y, target.creature->pos_x, me->character.pos_y,
                            me->character.pos_x)
         <= SKILL_RANGE[me->character.class][skill] && target.creature->state != CREATURE_STATE__DEAD) ||
        (target.type == UNIT_TYPE__CHARACTER &&
         SKILL_TARGET_TYPE[me->character.class][skill] == UNIT_TYPE__CHARACTER &&
         euclidean_distance(target.character->pos_y, target.character->pos_x, me->character.pos_y,
                            me->character.pos_x)
         <= SKILL_RANGE[me->character.class][skill]))
        return 1;
    return 0;
}


void *keep_connection_loop() {
    while (running) {
        sleep(1);
    }
    return NULL;
}