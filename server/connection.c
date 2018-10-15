#include "connection.h"

extern int sock;
extern int character_number;

ConnectionQueue *connection_queue_init(int max_connections) {
    ConnectionQueue *queue = malloc(sizeof(ConnectionQueue));
    queue->max_num = max_connections;
    queue->head = malloc(sizeof(Connection));
    queue->head->next = NULL;
    return queue;
}

int connection_queue_add_connection(ConnectionQueue *queue, int fd) {
    pthread_mutex_lock(&queue->queue_lock);
    if (queue->connection_num >= queue->max_num)
        return -1;
    Connection *conn = malloc(sizeof(Connection));
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
    pthread_mutex_unlock(&queue->queue_lock);
    return 0;
}

int remove_character(Connection *conn) {
    pthread_mutex_lock(&conn->character_data_lock);
    free(conn->character);
    conn->character = NULL;
    pthread_mutex_unlock(&conn->character_data_lock);
    character_number--;
    return 0;
}

int connection_queue_remove_connection(ConnectionQueue *queue, Connection *conn) {
    pthread_mutex_lock(&queue->queue_lock);
    if (conn->character != NULL) {
        remove_character(conn);
    }
    conn->prev->next = conn->next;
    if (conn->next != NULL) {
        conn->next->prev = conn->prev;
    }
    close(conn->fd);
    free(conn);
    printf("Connection freed\n");
    queue->connection_num--;
    pthread_mutex_unlock(&queue->queue_lock);
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
    resp.type = RESPONSE__TYPE__WELCOME_MESSAGE;
    resp.welcomemsg = &wm;
    wm.key = key;
    wm.id = conn->character->id;
    send_response(conn->fd, resp);
    return 0;
}

int send_login_fail(Connection *conn, RefuseLoginMessage__RefuseType type) {
    Response resp = RESPONSE__INIT;
    RefuseLoginMessage rl = REFUSE_LOGIN_MESSAGE__INIT;
    resp.type = RESPONSE__TYPE__REFUSE_LOGIN;
    resp.refuselogin = &rl;
    rl.type = type;
    send_response(conn->fd, resp);
    return 0;
}
