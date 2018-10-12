#include "connection.h"

extern int sock;

ConnectionQueue *connection_queue_init(int max_connections) {
    ConnectionQueue *queue = (ConnectionQueue *) malloc(sizeof(ConnectionQueue));
    queue->max_num = max_connections;
    queue->head = (Connection *) malloc(sizeof(Connection));
    queue->head->next = NULL;
    return queue;
}

int connection_queue_add_connection(ConnectionQueue *queue, int fd) {
    if (queue->connection_num >= queue->max_num)
        return -1;
    Connection *conn = (Connection *) malloc(sizeof(Connection));
    conn->fd = fd;
    conn->listened = 0;
    conn->key = (int) random();
    conn->next = queue->head->next;
    conn->prev = queue->head;
    time(&conn->last_request);
    conn->character = NULL;
    if (queue->head->next != NULL)
        queue->head->next->prev = conn;
    queue->head->next = conn;
    queue->connection_num++;
    return 0;
}

int connection_queue_remove_connection(ConnectionQueue *queue, Connection *conn) {
    conn->prev->next = conn->next;
    if (conn->next != NULL) {
        conn->next->prev = conn->prev;
    }
    close(conn->fd);
    free(conn);
    queue->connection_num--;
    return 1;
}

Connection *connection_queue_get_connection(ConnectionQueue *queue, int fd) {
    Connection *p = queue->head->next;
    while (p != NULL) {
        if (p->fd == fd)
            return p;
        p = p->next;
    }
    return NULL;
}

int send_welcome_message(Connection *conn, int key) {
    Response resp = RESPONSE__INIT;
    WelcomeMessage wm = WELCOME_MESSAGE__INIT;
    resp.type = RESPONSE__REQUEST_TYPE__WELCOME_MESSAGE;
    resp.welcomemsg = &wm;
    wm.key = key;
    send_response(conn->fd, resp);
    return 0;
}

int send_login_fail(Connection *conn) {
    Response resp = RESPONSE__INIT;
    resp.type = RESPONSE__REQUEST_TYPE__LOGIN_FAIL;
    send_response(conn->fd, resp);
    return 0;
}
