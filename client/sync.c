#include "sync.h"

int key;

void init_world(int id) {
    Response *resp = get_response(sock);
    int i;
    if (resp != NULL & resp->type == RESPONSE__TYPE__WORLD_STATE && resp->worldstate != NULL) {
        ch_list_head = malloc(sizeof(*ch_list_head));
        ch_list_head->next = NULL;
        for (int i = 0; i < resp->worldstate->n_charters; i++) {
            CharacterNode *node = malloc(sizeof(*node));
            node->next = ch_list_head->next;
            node->character.id = resp->worldstate->charters[i]->id;
            node->character.hp = resp->worldstate->charters[i]->hp;
            node->character.mp = resp->worldstate->charters[i]->mp;
            node->character.level = resp->worldstate->charters[i]->level;
            node->character.exp = resp->worldstate->charters[i]->exp;
            strcpy(node->character.nickname, resp->worldstate->charters[i]->nickname);
            node->character.class = (CharacterClass) resp->worldstate->charters[i]->class_;
            node->character.pos_y = resp->worldstate->charters[i]->pos_y;
            node->character.pos_x = resp->worldstate->charters[i]->pos_x;
            pthread_mutex_init(&node->character_data_lock, NULL);
            pthread_mutex_init(&node->next_lock, NULL);
            ch_list_head->next = node;
        }
        for (CharacterNode *node = ch_list_head->next; node != NULL; node = node->next)
            if (node->character.id == id) {
                me = node;
                break;
            }
        for (i = 0; i < resp->worldstate->n_creatures; i++) {
            creatures[i].id = i;
            creatures[i].hp = resp->worldstate->creatures[i]->hp;
            creatures[i].max_hp = resp->worldstate->creatures[i]->max_hp;
            creatures[i].pos_x = resp->worldstate->creatures[i]->pos_x;
            creatures[i].pos_y = resp->worldstate->creatures[i]->pos_y;
            creatures[i].category = resp->worldstate->creatures[i]->category;
            creatures[i].level = resp->worldstate->creatures[i]->level;
            creatures[i].ad = resp->worldstate->creatures[i]->ad;
            pthread_mutex_init(&creature_data_lock[i], NULL);
        }
        for (i = 0; i < NUM_SKILLS_PER_CLASS; i++) {
            last_cast_time[i].tv_sec = 0;
            last_cast_time[i].tv_usec = 0;
        }
    } else {
        print_bottom_center("Unknown error");
        exit_game();
    }
    response__free_unpacked(resp, NULL);
}

void sync_move() {
    Request req = REQUEST__INIT;
    MoveRequest mr = MOVE_REQUEST__INIT;
    req.type = REQUEST__TYPE__MOVE;
    req.move = &mr;
    mr.id = me->character.id;
    mr.key = key;
    mr.pos_x = me->character.pos_x;
    mr.pos_y = me->character.pos_y;
    send_request(sock, req);
}

void sync_skill(int skill) {
    Request req = REQUEST__INIT;
    SkillRequest sr = SKILL_REQUEST__INIT;
    req.type = REQUEST__TYPE__SKILL;
    req.skill = &sr;
    sr.skill = skill;
    if (SKILL_TARGET_TYPE[me->character.class][skill] == UNIT_TYPE__CHARACTER)
        sr.target_id = target.character->id;
    else sr.target_id = target.creature->id;
    send_request(sock, req);
}