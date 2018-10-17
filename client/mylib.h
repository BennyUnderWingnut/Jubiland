#ifndef CLIENT_MYLIB_H
#define CLIENT_MYLIB_H

#include <locale.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <panel.h>

#define CREATURE_NUM 1000
#define SELECT_LOOP 5
#define NUM_SKILLS_PER_CLASS 2

#define get_terrain_color_pair(terrain_type) (terrain_type+2)

#define print_center(str) (mvprintw(LINES / 2, (int) ((COLS - strlen(str)) / 2), "%s", str))

#define print_bottom_center(str) {\
    mvhline(LINES - 1, 0, ' ', COLS);\
    mvprintw(LINES - 1, (int) ((COLS - strlen(str)) / 2), "%s", str);\
}

#define print_top_center(str) {\
    mvhline(0, 0, ' ', COLS);\
    mvprintw(0, (int) ((COLS - strlen(str)) / 2), "%s", str);\
}

void exit_game();

void create_detached_thread(void *(*start_routine)(void *), void *arg);

int sock, key;

WINDOW *default_window, *skill_window, *target_window;
PANEL *skill_panel, *target_info_panel;

double get_euclidean_distance(int y1, int x1, int y2, int x2);

#endif //CLIENT_MYLIB_H
