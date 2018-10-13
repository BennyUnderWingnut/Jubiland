#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "connection.h"
#include "character.h"

int add_character(ConnectionQueue *connectionQueue, Connection *connection, int class, char *nickname);

int remove_character(Connection *conn);

int send_world_status(ConnectionQueue *connectionQueue, Connection *connection);

#endif //SERVER_GAME_H
