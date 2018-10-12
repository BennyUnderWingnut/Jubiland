#include "login.h"

extern WINDOW *default_window;
extern int sock;
extern int key;
extern CharacterClass class;
extern unsigned long id;
extern char nickname[];

void choose_class(void);

void input_nickname(void);

int request_login(void);

int get_login_status(void);

void login() {
    choose_class();
    get_login_status();
}

void refresh_class_options(int cls) {
    clear();
    print_bottom_center(MSG_EXIT_HINT);
    print_top_center(MSG_CHOOSE_CLASS_HINT);
    char str[20];
    for (int i = 0; i < CLASS_TYPES; ++i) {
        if (i == cls)
            attron(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
        sprintf(str, "%10s %-6s", CLASS_NAME[i], CLASS_ICON[i]);
        mvaddstr(LINES / 2 - CLASS_TYPES / 2 + i, (int) ((COLS - strlen(str)) / 2), str);
        if (i == cls)
            attroff(COLOR_PAIR(COLOR_PAIR__TEXT_SELECTED));
    }
}

void refresh_nickname_input() {
    char buf[100];
    sprintf(buf, "%s%s", MSG_INPUT_HINT, nickname);
    clear();
    mvhline(LINES - 1, 0, ' ', COLS);
    print_bottom_center(MSG_BACK_HINT);
    mvhline(LINES / 2, 0, ' ', COLS);
    print_center(buf);
}

void choose_class() {
    curs_set(0);
    int ch;
    int cls = 0;
    do {
        refresh_class_options(cls);
        ch = getch();
        switch (ch) {
            case KEY_UP:
                cls = (cls + CLASS_TYPES - 1) % CLASS_TYPES;
                break;
            case KEY_DOWN:
                cls = (cls + 1) % CLASS_TYPES;
                break;
            case KEY_ENTER:
            case 10:
                class = cls;
                input_nickname();
            default:;
        }
    } while (ch != 27);
    exit_game();
}

void input_nickname() {
    curs_set(1);
    int ch, y, x;
    /* print the message at the center of the screen */
    refresh_nickname_input();
    do {
        ch = getch();
        if (!(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == KEY_LEFT ||
              ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN || ch == 127 || ch == KEY_BACKSPACE || ch == 10 ||
              ch == KEY_ENTER || ch == 27)) {
            refresh_nickname_input();
            getyx(default_window, y, x);
            print_bottom_center(MSG_NICKNAME_LIMIT);
            move(y, x);
            continue;
        }
        switch (ch) {
            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_UP:
            case KEY_DOWN:
                break;
            case 127:
            case KEY_BACKSPACE:
                if (strlen(nickname) > 0) {
                    nickname[strlen(nickname) - 1] = '\0';
                    refresh_nickname_input();
                }
                break;
            case 10:
            case KEY_ENTER:
                if (strlen(nickname) < 6) {
                    getyx(default_window, y, x);
                    print_bottom_center(MSG_NAME_TOO_SHORT);
                    move(y, x);
                } else if (strlen(nickname) > 15) {
                    getyx(default_window, y, x);
                    print_bottom_center(MSG_NAME_TOO_LONG);
                    move(y, x);
                } else {
                    sock = connect_to_server(HOST_NAME, PORT_NUM);
                    if (sock == -1) {
                        getyx(default_window, y, x);
                        print_bottom_center(MSG_CANNOT_CONNECT);
                        move(y, x);
                    } else {
                        request_login();
                        return;
                    }
                }
                break;
            case 27:
                choose_class();
                break;
            default:
                if (strlen(nickname) >= 15) {
                    getyx(default_window, y, x);
                    print_bottom_center(MSG_NAME_TOO_LONG);
                    move(y, x);
                } else {
                    nickname[strlen(nickname)] = (char) ch;
                    refresh_nickname_input();
                }
        }
    } while (ch != 27);
    // go back
}

int request_login() {
    Request req = REQUEST__INIT;
    LoginRequest lr = LOGIN_REQUEST__INIT;
    req.type = REQUEST_TYPE__LOGIN;
    req.login = &lr;
    lr.nickname = nickname;
    lr.class_ = CHARACTER_CLASS__WARRIOR;
    send_request(sock, req);
    curs_set(0);
    print_bottom_center(MSG_WAITING_RESPONSE);
    return 0;
}

int get_login_status() {
    // Read login response message
    Response *resp = get_response(sock);
    if (resp->type == RESPONSE__REQUEST_TYPE__LOGIN_FAIL) return -1;
    if (resp->type == RESPONSE__REQUEST_TYPE__WELCOME_MESSAGE) {
        id = resp->welcomemsg->id;
        key = resp->welcomemsg->key;
    }
    return 0;
}