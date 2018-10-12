#ifndef SERVER_LOGIN_H
#define SERVER_LOGIN_H

#include <pthread.h>
#include <sys/socket.h>
#include <stdlib.h>
//#include "thread_pool.h"

#include "thpool.h"
#include "socklib.h"
#include "request.pb-c.h"
#include "response.pb-c.h"
#include "character.h"

typedef struct connection {
    int fd;
    int listened; // socket is being listened
    int key; // key used to verify client
    time_t last_request;
    pthread_mutex_t character_data_lock;
    Character *character;
    struct connection *prev;
    struct connection *next;
} Connection;

typedef struct connection_queue {
    int connection_num; // current number of connections
    int max_num; // max number of connections in queue
    Connection *head; // pointer to the header of the queue
} ConnectionQueue;

ConnectionQueue *connection_queue_init(int max_connections);

int connection_queue_add_connection(ConnectionQueue *queue, int fd);

int connection_queue_remove_connection(ConnectionQueue *queue, Connection *conn);

Connection *connection_queue_get_connection(ConnectionQueue *queue, int fd);

int send_login_fail(Connection *conn);

int send_welcome_message(Connection *conn, int key);

#endif //SERVER_LOGIN_H
