#include "sync.h"

int key;
extern int sock, map[1024][1024];
extern CharacterNode *me;
extern CharacterNode *chListHead;

void init_world(int id) {
    Response *resp = get_response(sock);
    if (resp != NULL & resp->type == RESPONSE__REQUEST_TYPE__WORLD_STATE && resp->worldstate != NULL) {
        chListHead = malloc(sizeof(*chListHead));
        chListHead->next = NULL;
        for (int i = 0; i < resp->worldstate->n_charters; i++) {
            CharacterNode *node = malloc(sizeof(*node));
            node->next = chListHead->next;
            node->character.id = resp->worldstate->charters[i]->id;
            node->character.hp = resp->worldstate->charters[i]->hp;
            node->character.mp = resp->worldstate->charters[i]->mp;
            node->character.level = resp->worldstate->charters[i]->level;
            node->character.exp = resp->worldstate->charters[i]->exp;
            strcpy(node->character.nickname, resp->worldstate->charters[i]->nickname);
            node->character.class = (CharacterClass) resp->worldstate->charters[i]->class_;
            node->character.pos_y = resp->worldstate->charters[i]->pos_y;
            node->character.pos_x = resp->worldstate->charters[i]->pos_x;
            chListHead->next = node;
        }
        for (CharacterNode *node = chListHead->next; node != NULL; node = node->next)
            if (node->character.id == id) {
                me = node;
                break;
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
    req.type = REQUEST_TYPE__MOVE;
    req.move = &mr;
    mr.id = me->character.id;
    mr.key = key;
    mr.pos_x = me->character.pos_x;
    mr.pos_y = me->character.pos_y;
    send_request(sock, req);
}