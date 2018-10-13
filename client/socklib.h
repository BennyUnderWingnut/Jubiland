#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <stdlib.h>
#include "request.pb-c.h"
#include "response.pb-c.h"

#define HOST_NAME "localhost"
#define PORT_NUM 8888
#define MAX_MSG_SIZE 1048576
#define BUFF_SIZE 4096

int connect_to_server(char *host, int port);

Response *get_response(int fd);

int send_request(int sock, Request req);

#endif //CLIENT_SOCKET_H
