#include "game.h"

extern ConnectionQueue connectionQueue;

int add_character(Connection *conn, int class) {
    Character *character;
    if ((character = init_character(class)) == NULL) {
        return -1;
    }
    conn->character = character;
    return 0;
}

int remove_character(Connection conn) {
    // TODO
    return 0;
}

int send_world_status() {
    Connection *conn = connectionQueue.head->next;
    Response resp = RESPONSE__INIT;
    resp.type = RESPONSE__REQUEST_TYPE__WORLD_STATE;
    while (conn != NULL) {
        // TODO
    }
}