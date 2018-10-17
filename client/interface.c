#include "interface.h"

void print_status_panel() {
    char buf[100];
    sprintf(buf, "%s %s  Lv. %d  EXP %d/%d  ❤️  %d/%d  🔮 %d/%d",
            CLASS_ICON[me->character.class], me->character.nickname, me->character.level, me->character.exp,
            EXP_TO_UPGRADE[me->character.level], me->character.hp,
            CHARACTER_MAX_HP[me->character.class][me->character.level],
            me->character.mp, CHARACTER_MAX_MP[me->character.class][me->character.level]);
    hline(' ', 0);
    print_top_center(buf);
}

void print_target_panel() {
    char buf[200];
    bzero(buf, 200);
    if ((int) target.type == -1) return;
    else if (target.type == UNIT_TYPE__CHARACTER) {
        sprintf(buf, " %s %s  Lv. %d  EXP %d/%d  ❤️  %d/%d  🔮 %d/%d ",
                CLASS_ICON[target.character->class], target.character->nickname,
                target.character->level, target.character->exp,
                EXP_TO_UPGRADE[target.character->level], target.character->hp,
                CHARACTER_MAX_HP[target.character->class][target.character->level],
                target.character->mp,
                CHARACTER_MAX_MP[target.character->class][target.character->level]);
    } else {
        pthread_mutex_lock(&creature_data_lock[target.creature->id]);
        sprintf(buf, "  %s %s Lv. %d  HP %d/%d  ",
                CREATURE_CATEGORY_ICON[target.creature->category],
                CREATURE_NAME[target.creature->category][target.creature->level / 5],
                target.creature->level, target.creature->hp, target.creature->max_hp);
        pthread_mutex_unlock(&creature_data_lock[target.creature->id]);
    }
    mvaddstr(2, (int) ((COLS - strlen(buf)) / 2), buf);
}

void print_skill_panel() {
    char buf[100], str[100] = "    ";
    float cd;
    for (int i = 0; i < NUM_SKILLS_PER_CLASS; i++) {
        cd = get_remaining_cd(i);
        if (cd == 0) sprintf(buf, "%d %s    ", i + 1, SKILL_ICON[me->character.class][i]);
        else
            sprintf(buf, "%d %s %g    ", i, SKILL_ICON[me->character.class][i], cd);
        strcat(str, buf);
    }
    mvhline(LINES - 1, 0, ' ', COLS);
    mvaddstr(LINES - 1, (int) ((COLS - strlen(str)) / 2), str);
}

int has_been_selected(UnitType type, int id) {
    if (type == UNIT_TYPE__CHARACTER)
        for (int i = 0; i < SELECT_LOOP - 1; i++) {
            if (selected_targets[i].type == type && selected_targets[i].character->id == id)
                return 1;
        }
    else
        for (int i = 0; i < SELECT_LOOP - 1; i++) {
            if (selected_targets[i].type == type && selected_targets[i].creature->id == id)
                return 1;
        }
    return 0;
}

Unit select_a_target() {
    int i;
    CharacterNode *ch = ch_list_head, *prev;
    Unit target;
    target.type = -1;
    double dist, min_dist = 999999;
    while (ch->next != NULL) {
        pthread_mutex_lock(&ch->next_lock);
        prev = ch;
        ch = ch->next;
        pthread_mutex_unlock(&prev->next_lock);
        if (ch->character.id == me->character.id || has_been_selected(UNIT_TYPE__CHARACTER, ch->character.id)) continue;
        pthread_mutex_lock(&ch->character_data_lock);
        dist = get_euclidean_distance(ch->character.pos_y, ch->character.pos_x, me->character.pos_y,
                                      me->character.pos_x);
        if (dist < min_dist) {
            target.type = UNIT_TYPE__CHARACTER;
            target.character = &ch->character;
            min_dist = dist;
        }
        pthread_mutex_unlock(&ch->character_data_lock);
    }
    for (i = 0; i < CREATURE_NUM; i++) {
        if (has_been_selected(UNIT_TYPE__CREATURE, i)) continue;
        pthread_mutex_lock(&creature_data_lock[i]);
        dist = get_euclidean_distance(creatures[i].pos_y, creatures[i].pos_x, me->character.pos_y, me->character.pos_x);
        if (dist < min_dist) {
            target.type = UNIT_TYPE__CREATURE;
            target.creature = &creatures[i];
            min_dist = dist;
        }
        pthread_mutex_unlock(&creature_data_lock[i]);
    }
    selected_number = selected_number % (SELECT_LOOP - 1) + 1;
    selected_targets[selected_number - 1] = target;
    return target;
}

void init_interface() {
    target.type = -1;
    for (int i = 0; i < SELECT_LOOP - 1; i++) {
        selected_targets[i].type = -1;
    }
}