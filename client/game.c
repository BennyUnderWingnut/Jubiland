#include "game.h"

extern WINDOW *default_window;
const char *SHAPES[] = {SHAPE_EMPTY, SHAPE_GRASS, SHAPE_WATER, SHAPE_MOUNTAIN};
CharacterNode *chListHead;
CharacterNode *me;
int running = 1;

int is_move_okay(int y, int x);

void update_graph(void);

int is_move_okay(int y, int x) {
    if (y < 0 || y > MAP_LINES - 1 || x < 0 || x > MAP_COLS - 1) return 0;
    return map[y][x] == TERRAIN_GRASS;
}

void end_game() {
    running = 0;
    exit_game();
}

void main_loop() {
    curs_set(0);/* hide cursor */
    int ch, rv;
    fd_set set;
    struct timeval timeout;
    while (running) {
        update_graph();
        refresh();

        FD_ZERO(&set);
        FD_SET(0, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = REFRESH_USEC;
        rv = select(1, &set, NULL, NULL, &timeout);
        if (rv == 0) continue;
        else if (rv == -1) end_game();
        ch = getch();
        switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
                if (is_move_okay(me->character.pos_y - 1, me->character.pos_x)) me->character.pos_y--;
                sync_move();
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                if (is_move_okay(me->character.pos_y + 1, me->character.pos_x)) me->character.pos_y++;
                sync_move();
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                if (is_move_okay(me->character.pos_y, me->character.pos_x - 1)) me->character.pos_x--;
                sync_move();
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                if (is_move_okay(me->character.pos_y, me->character.pos_x + 1)) me->character.pos_x++;
                sync_move();
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
    wclear(default_window);
    int i, j, map_y, map_x;
    for (i = 0; i < LINES; i++) {
        for (j = 0; j < COLS; j += 2) {
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
    for (CharacterNode *node = chListHead->next; node != NULL; node = node->next) {
        map_y = node->character.pos_y;
        map_x = node->character.pos_x;
        attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
        int screen_y = map_y - me->character.pos_y + (LINES - 1) / 2;
        int screen_x = 2 * (map_x - me->character.pos_x + (COLS - 1) / 4);
        mvaddstr(screen_y, screen_x, SHAPE_CHARACTER);
        attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
    }
}

void *listen_events(void *sockptr) {
    int sock = *(int *) sockptr;
    fd_set set;
    Response *resp;
    MoveEventMessage *mem;
    NewcomerEventMessage *nem;
    int rv, i;
    CharacterNode *ch;
    while (running) {
        FD_ZERO(&set);
        FD_SET(sock, &set);
        rv = select(sock + 1, &set, NULL, NULL, 0);
        if (rv == -1) end_game();
        resp = get_response(sock);
        if (resp == NULL) end_game();
        else if (resp->type == RESPONSE__REQUEST_TYPE__EVENTS) {
            if (resp->events == NULL) end_game();
            for (i = 0; i < resp->events->n_moveevents; i++) {
                mem = resp->events->moveevents[i];
                for (ch = chListHead->next; ch != NULL; ch = ch->next)
                    if (ch->character.id == mem->id) {
                        pthread_mutex_lock(&ch->character_data_mutex);
                        ch->character.pos_y = mem->pos_y;
                        ch->character.pos_x = mem->pos_x;
                        pthread_mutex_unlock(&ch->character_data_mutex);
                        break;
                    }
            }
            for (i = 0; i < resp->events->n_newcomerevents; i++) {
                nem = resp->events->newcomerevents[i];
                CharacterNode *node = malloc(sizeof(*node));
                node->next = chListHead->next;
                node->character.id = nem->id;
                node->character.hp = nem->hp;
                node->character.mp = nem->mp;
                node->character.level = nem->level;
                node->character.exp = nem->exp;
                strcpy(node->character.nickname, nem->nickname);
                node->character.class = (CharacterClass) nem->class_;
                node->character.pos_y = nem->pos_y;
                node->character.pos_x = nem->pos_x;
                chListHead->next = node;
            }
        }
        response__free_unpacked(resp, NULL);
    }
    return NULL;
}