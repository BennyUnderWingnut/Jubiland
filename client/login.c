#include "login.h"

int choose_class(char *nickname);

int input_nickname(CharacterClass class, char *nickname, int *id);

int request_login(CharacterClass class, char *nickname);

int get_login_status(int *id);

int login() {
    char nickname[16] = "";
    return choose_class(nickname);
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

void refresh_nickname_input(char *nickname) {
    char buf[100];
    sprintf(buf, "%s%s", MSG_INPUT_HINT, nickname);
    clear();
    mvhline(LINES - 1, 0, ' ', COLS);
    print_bottom_center(MSG_BACK_HINT);
    mvhline(LINES / 2, 0, ' ', COLS);
    print_center(buf);
}

int choose_class(char *nickname) {
    curs_set(0);
    int ch, id;
    int class = 0;
    do {
        refresh_class_options(class);
        ch = getch();
        switch (ch) {
            case KEY_UP:
                class = (class + CLASS_TYPES - 1) % CLASS_TYPES;
                break;
            case KEY_DOWN:
                class = (class + 1) % CLASS_TYPES;
                break;
            case KEY_ENTER:
            case 10:
                if (input_nickname(class, nickname, &id) == 0)
                    return id;
            default:;
        }
    } while (ch != 27);
    exit_game();
    return 0;
}

int input_nickname(CharacterClass class, char *nickname, int *id) {
    curs_set(1);
    int ch, y, x, rv;
    /* print the message at the center of the screen */
    refresh_nickname_input(nickname);
    do {
        ch = getch();
        if (!(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == KEY_LEFT ||
              ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN || ch == 127 || ch == KEY_BACKSPACE || ch == 10 ||
              ch == KEY_ENTER || ch == 27)) {
            refresh_nickname_input(nickname);
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
                    refresh_nickname_input(nickname);
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
                    if (request_login(class, nickname) == -1) {
                        getyx(default_window, y, x);
                        print_bottom_center(MSG_CANNOT_CONNECT);
                        move(y, x);
                        break;
                    };
                    rv = get_login_status(id);
                    if (rv == 0) {
                        print_bottom_center(MSG_LOGIN_SUCCESS);
                        return 0;
                    } else if (rv == REFUSE_LOGIN_MESSAGE__REFUSE_TYPE__NICKNAME_TAKEN) {
                        getyx(default_window, y, x);
                        print_bottom_center(MSG_NICKNAME_TAKEN);
                        move(y, x);
                    } else {
                        getyx(default_window, y, x);
                        print_bottom_center(MSG_UNKNOWN_ERROR);
                        move(y, x);
                    }
                }
                break;
            case 27:
                break;
            default:
                if (strlen(nickname) >= 15) {
                    getyx(default_window, y, x);
                    print_bottom_center(MSG_NAME_TOO_LONG);
                    move(y, x);
                } else {
                    nickname[strlen(nickname)] = (char) ch;
                    refresh_nickname_input(nickname);
                }
        }
    } while (ch != 27);
    return 1;
}

int request_login(CharacterClass class, char *nickname) {
    if ((sock = connect_to_server(HOST_NAME, PORT_NUM)) == -1) return -1;
    Request req = REQUEST__INIT;
    LoginRequest lr = LOGIN_REQUEST__INIT;
    req.type = REQUEST__TYPE__LOGIN;
    req.login = &lr;
    lr.nickname = nickname;
    lr.class_ = class;
    send_request(sock, req);
    curs_set(0);
    print_bottom_center(MSG_WAITING_RESPONSE);
    return 0;
}

int get_login_status(int *id) {
    // Read login response message
    Response *resp = get_response(sock);
    if (resp->type == RESPONSE__TYPE__REFUSE_LOGIN) return resp->refuselogin->type;
    if (resp->type == RESPONSE__TYPE__WELCOME_MESSAGE) {
        *id = resp->welcomemsg->id;
        key = resp->welcomemsg->key;
    }
    response__free_unpacked(resp, NULL);
    return 0;
}