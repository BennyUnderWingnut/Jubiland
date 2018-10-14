#include "sync.h"

int key;
extern int x, y, my_id, sock, map[1024][1024];

void init_world() {
    int map_y, map_x;
    Response *resp = get_response(sock);
    if (resp != NULL & resp->type == RESPONSE__REQUEST_TYPE__WORLD_STATE && resp->worldstate != NULL) {
        for (int i = 0; i < resp->worldstate->n_charters; i++)
            if (my_id == resp->worldstate->charters[i]->id) {
                y = resp->worldstate->charters[i]->pos_y;
                x = resp->worldstate->charters[i]->pos_x;
            }
        for (int i = 0; i < resp->worldstate->n_charters; i++) {
            map_y = resp->worldstate->charters[i]->pos_y;
            map_x = resp->worldstate->charters[i]->pos_x;
            attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            int screen_y = map_y - y + (LINES - 1) / 2;
            int screen_x = 2 * (map_x - x + (COLS - 1) / 4);
            mvaddstr(screen_y, screen_x, SHAPE_CHARACTER);
            attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
        }
    } else {
        print_bottom_center("Unknown error");
        exit_game();
    }
}

int sync_move() {
    Request req = REQUEST__INIT;

}