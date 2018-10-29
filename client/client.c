#include "game.h"

void init_curses(void);

void bind_signals(void);

void keep_connection(void);

int main(void) {
    init_curses();
    int id = login();
    init_map();
    init_world(id);
    init_interface();
    bind_signals();
    keep_connection();
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
    init_pair(COLOR_PAIR__TEXT_ATTENTION, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR__TEXT_SELECTED, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR__TERRAIN_EMPTY, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR__TERRAIN_GRASS, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_WATER, COLOR_CYAN, COLOR_BLUE);
    init_pair(COLOR_PAIR__TERRAIN_MOUNTAIN, COLOR_WHITE, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_ROSE, COLOR_RED, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_TULIP, COLOR_MAGENTA, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_TREE, COLOR_YELLOW, COLOR_GREEN);
    init_pair(COLOR_PAIR__TERRAIN_ISLAND, COLOR_YELLOW, COLOR_BLUE);
    init_pair(COLOR_PAIR__TERRAIN_CAMP, COLOR_RED, COLOR_GREEN);
    clear();
}

void bind_signals() {
    signal(SIGINT, end_game);
    signal(SIGTERM, end_game);
}

void keep_connection(void) {
    pthread_mutex_init(&sock_write_lock, NULL);
    signal(SIGALRM, send_keep_connection_request);

    create_detached_thread(keep_connection_loop, NULL);

    sigset_t sigset;
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    set_ticker(60000);
};