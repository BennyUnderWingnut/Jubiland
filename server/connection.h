#ifndef SERVER_LOGIN_H
#define SERVER_LOGIN_H


//#include "thread_pool.h"
#include "thpool.h"
#include "socklib.h"
#include "request.pb-c.h"
#include "response.pb-c.h"
#include "character.h"
#include "event.h"

#define CONNECTION_TIMEOUT_SEC 300

typedef struct _Connection {
    int fd;
    int listened; // socket is being listened
    int key; // key used to verify client
    struct timeval last_keep_connection;
    pthread_mutex_t character_data_lock;
    Character *character;
    struct _Connection *prev;
    struct _Connection *next;
} Connection;

typedef struct connection_queue {
    int connection_num; // current number of connections
    int max_num; // max number of connections in queue
    Connection *head; // pointer to the header of the queue
    pthread_mutex_t queue_lock;
} ConnectionQueue;

ConnectionQueue *connection_queue_init(int max_connections);

int connection_queue_add_connection(ConnectionQueue *queue, int fd);

int connection_queue_remove_connection(ConnectionQueue *queue, Connection *conn);

Connection *connection_queue_get_connection(ConnectionQueue *queue, int fd);

int send_login_fail(Connection *conn, RefuseLoginMessage__RefuseType type);

int send_welcome_message(Connection *conn, int key);

Connection *get_connection_by_id(ConnectionQueue *queue, int id);

int reset_key(Connection *conn);

void *remove_not_responding_connections_loop();

#endif //SERVER_LOGIN_H
