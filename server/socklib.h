#ifndef SERVER_SOCKLIB_H
#define SERVER_SOCKLIB_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#include <pthread.h>
#include <stdlib.h>
#include "request.pb-c.h"
#include "response.pb-c.h"

#define MAX_MSG_SIZE 65536
#define BUFF_SIZE 4096

Request *get_request(int fd, int *stat);

int make_server_socket(int portnum);

int send_response(int fd, Response resp);

void handle_sigpipe();

#endif //SERVER_SOCKLIB_H
