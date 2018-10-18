#include "connection.h"

extern int sock;
extern int character_number;
extern LogoutEventQueue *logoutEventQueue;
extern int running;
extern pthread_mutex_t remove_not_responding_connections_lock;
extern int wakeup_count;
extern ConnectionQueue *connectionQueue;

ConnectionQueue *connection_queue_init(int max_connections) {
    ConnectionQueue *queue = malloc(sizeof(ConnectionQueue));
    queue->max_num = max_connections;
    queue->head = malloc(sizeof(Connection));
    queue->head->next = NULL;
    pthread_mutex_init(&queue->queue_lock, NULL);
    return queue;
}

int connection_queue_add_connection(ConnectionQueue *queue, int fd) {
    pthread_mutex_lock(&queue->queue_lock);
    if (queue->connection_num >= queue->max_num) {
        pthread_mutex_unlock(&queue->queue_lock);
        return -1;
    }
    Connection *conn = malloc(sizeof(Connection));
    conn->fd = fd;
    conn->listened = 0;
    conn->key = (int) random();
    conn->next = queue->head->next;
    conn->prev = queue->head;
    gettimeofday(&conn->last_keep_connection, NULL);
    conn->character = NULL;
    pthread_mutex_init(&conn->character_data_lock, NULL);
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
        add_logout_event(logoutEventQueue, conn->character->id);
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

Connection *get_connection_by_id(ConnectionQueue *queue, int id) {
    Connection *conn = queue->head->next;
    while (conn != NULL) {
        if (conn->character->id == id) return conn;
        conn = conn->next;
    }
    return NULL;
}

int reset_key(Connection *conn) {
    conn->key = (int) random();
    Response resp = RESPONSE__INIT;
    ResetKeyMessage rk = RESET_KEY_MESSAGE__INIT;
    resp.type = RESPONSE__TYPE__RESET_KEY;
    resp.resetkey = &rk;
    rk.key = conn->key;
    send_response(conn->fd, resp);
    return 0;
};

void *remove_not_responding_connections_loop() {
    while (running) {
        pthread_mutex_lock(&remove_not_responding_connections_lock);
        if (wakeup_count % 1000 == 0) {
            pthread_mutex_lock(&connectionQueue->queue_lock);
            Connection *conn = connectionQueue->head->next, *next;
            while (conn != NULL) {
                next = conn->next;
                if (time_pass_since(conn->last_keep_connection) > CONNECTION_TIMEOUT_SEC)
                    connection_queue_remove_connection(connectionQueue, conn);
                conn = next;
            }
            pthread_mutex_unlock(&connectionQueue->queue_lock);
        }
    }
    return NULL;
}