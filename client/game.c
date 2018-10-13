#include "game.h"

WINDOW *default_window;
int map[1024][1024] = {0};
int y, x, sock, key = 0;
unsigned long id;
CharacterClass class = 0;
char nickname[15] = "";
const char *SHAPES[] = {"  ", SHAPE_EMPTY, SHAPE_GRASS, SHAPE_WATER, SHAPE_MOUNTAIN};

int main(void) {
    init_curses();
    login();
    init_map();
    init_world();
    main_loop();
    endwin();
    exit(0);
}

void init_curses() {
    /* initialize curses */

    setlocale(LC_ALL, "");
    default_window = initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    /* initialize colors */

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    init_pair(COLOR_PAIR__TEXT_SELECTED, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR__TERRAIN_EMPTY, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR__TERRAIN_GRASS, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_WATER, COLOR_CYAN, COLOR_BLUE);
    init_pair(COLOR_PAIR__TERRAIN_MOUNTAIN, COLOR_BLACK, COLOR_WHITE);
    clear();
}

int is_move_okay(int y, int x) {
    /* return true if the space is okay to move into */
    return map[y][x] == TERRAIN_GRASS;
}

void update_graph(void) {
    int i, j, map_y, map_x;
    for (i = 0; i < LINES; i++) {
        for (j = 0; j < COLS; j += 2) {
            map_y = y + i - (LINES - 1) / 2;
            map_x = x + j / 2 - (COLS - 1) / 4 * 2;
            if (map_y < 0 || map_y >= MAP_LINES || map_x < 0 || map_x >= MAP_COLS) {
                attron(COLOR_PAIR(TERRAIN_EMPTY));
                mvaddstr(i, j * 2, SHAPE_EMPTY);
                attroff(COLOR_PAIR(TERRAIN_EMPTY));
            } else {
                attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
                mvaddstr(i * 2, j * 2, SHAPES[map[map_y][map_x]]);
                attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            }
        }
    }
    attron(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
    mvaddstr((LINES - 1) / 2, (COLS - 1) / 4 * 2, SHAPE_PLAYER);
    attroff(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
}

void init_world() {
    int map_y, map_x;
    Response *resp = get_response(sock);
    if (resp != NULL & resp->type == RESPONSE__REQUEST_TYPE__WORLD_STATE && resp->worldstate != NULL) {
        for (int i = 0; i < resp->worldstate->n_charters; i++)
            if (id == resp->worldstate->charters[i]->id) {
                y = resp->worldstate->charters[i]->pos_y;
                x = resp->worldstate->charters[i]->pos_x;
            }
        for (int i = 0; i < resp->worldstate->n_charters; i++) {
            map_y = resp->worldstate->charters[i]->pos_y;
            map_x = resp->worldstate->charters[i]->pos_x;
            attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            int screen_y = map_y - y + (LINES - 1) / 2;
            int screen_x = 2 * (map_x - x + (COLS - 1) / 4 * 2);
            mvaddstr(screen_y, screen_x, SHAPE_PLAYER);
            attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
        }
    } else {
        print_bottom_center("Unknown error");
        exit_game();
    }
}

void main_loop() {
    curs_set(0);/* hide cursor */
    int ch;
    do {
        /* by default, you get a blinking cursor - use it to
           indicate player * */
        update_graph();
        refresh();

        ch = getch();
        switch (ch) {
            case KEY_UP:
            case 'w':
            case 'W':
                if ((y > 0) && is_move_okay(y - 1, x)) {
                    y = y - 1;
                }
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                if ((y < MAP_LINES - 1) && is_move_okay(y + 1, x)) {
                    y = y + 1;
                }
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                if ((x > 0) && is_move_okay(y, x - 1)) {
                    x = x - 2;
                }
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                if ((x < MAP_COLS - 1) && is_move_okay(y, x + 1)) {
                    x = x + 2;
                }
                break;
            default:;
        }
    } while ((ch != 'q') && (ch != 'Q'));
}