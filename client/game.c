#include "game.h"

extern WINDOW *default_window;
int map[1024][1024] = {0};
int y, x;
const char *SHAPES[] = {SHAPE_EMPTY, SHAPE_GRASS, SHAPE_WATER, SHAPE_MOUNTAIN};
CharacterClass class = 0;

int is_move_okay(int y, int x);

void update_graph(void);

int is_move_okay(int y, int x) {
    /* return true if the space is okay to move into */
    return map[y][x] == TERRAIN_GRASS;
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
                    y--;
                }
                break;
            case KEY_DOWN:
            case 's':
            case 'S':
                if ((y < MAP_LINES - 1) && is_move_okay(y + 1, x)) {
                    y++;
                }
                break;
            case KEY_LEFT:
            case 'a':
            case 'A':
                if ((x > 0) && is_move_okay(y, x - 1)) {
                    x--;
                }
                break;
            case KEY_RIGHT:
            case 'd':
            case 'D':
                if ((x < MAP_COLS - 1) && is_move_okay(y, x + 1)) {
                    x++;
                }
                break;
            default:;
        }
    } while ((ch != 'q') && (ch != 'Q'));
}

void update_graph(void) {
    wclear(default_window);
    int i, j, map_y, map_x;
    for (i = 0; i < LINES; i++) {
        for (j = 0; j < COLS; j += 2) {
            map_y = y + i - (LINES - 1) / 2;
            map_x = x + j / 2 - (COLS - 1) / 4;
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
    attron(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
    mvaddstr((LINES - 1) / 2, (COLS - 1) / 4 * 2, SHAPE_CHARACTER);
    attroff(COLOR_PAIR(get_terrain_color_pair(map[y][x])));
}
