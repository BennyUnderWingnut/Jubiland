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

#define REFRESH_INTERVAL_USEC 20000 // 刷新间隔
#define MOVE_INTERVAL_USEC 160000 // 移动间隔

#define COLOR_PAIR__TEXT_SELECTED 1
#define COLOR_PAIR__TERRAIN_EMPTY 2
#define COLOR_PAIR__TERRAIN_GRASS 3
#define COLOR_PAIR__TERRAIN_WATER 4
#define COLOR_PAIR__TERRAIN_MOUNTAIN 5

#define SHAPE_EMPTY     "  "
#define SHAPE_GRASS     "  "
#define SHAPE_WATER     "🌊"
#define SHAPE_MOUNTAIN  "🗻"
#define SHAPE_CHARACTER    "👦"
#define SHAPE_ME "🤴"

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

#endif //CLIENT_MYLIB_H
