#include "mylib.h"
#include "map.h"
#include "login.h"
#include "socklib.h"
#include "sync.h"
#include "game.h"

WINDOW *default_window;
int sock, my_id;
char nickname[15] = "";

void init_curses(void);

void bind_signals(void);

int main(void) {
    init_curses();
    int id = login();
    init_map();
    init_world(id);
    bind_signals();
    create_detached_thread(listen_events, (void *) &sock);
    main_loop();
}

void init_curses() {
    /* initialize curses */
    setlocale(LC_ALL, "");
    default_window = initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
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
    init_pair(COLOR_PAIR__TERRAIN_MOUNTAIN, COLOR_WHITE, COLOR_GREEN);
    clear();
}

void bind_signals() {
    signal(SIGINT, end_game);
    signal(SIGTERM, end_game);
}