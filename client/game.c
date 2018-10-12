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

    y = 5; // TODO
    x = 10; // TODO
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
        for (j = 0; j < COLS; j++) {
            map_y = y + i - (LINES - 1) / 2;
            map_x = x + j - (COLS - 1) / 2;
            if (map_y < 0 || map_y >= MAP_LINES || map_x < 0 || map_x >= MAP_COLS) {
                attron(COLOR_PAIR(TERRAIN_EMPTY));
                mvaddstr(i * 2, j, SHAPE_EMPTY);
                attroff(COLOR_PAIR(TERRAIN_EMPTY));
            } else {
                attron(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
                mvaddstr(i * 2, j, SHAPES[map[map_y][map_x]]);
                attroff(COLOR_PAIR(get_terrain_color_pair(map[map_y][map_x])));
            }
        }
    }
    attron(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
    mvaddstr((LINES - 1) / 2, (COLS - 1) / 2, SHAPE_PLAYER);
    attroff(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
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
                    x = x - 1;
                }
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                if ((x < MAP_COLS - 1) && is_move_okay(y, x + 1)) {
                    x = x + 1;
                }
                break;
            default:;
        }
    } while ((ch != 'q') && (ch != 'Q'));
}