#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "connection.h"
#include "event.h"
#include "character.h"
#include "ai.h"

int add_character(ConnectionQueue *connectionQueue, Connection *connection, int class, char *nickname);

int remove_character(Connection *conn);

int send_world_status(ConnectionQueue *connectionQueue, Connection *connection);

int move_character(Connection *connection, int y, int x);

int use_skill(Connection *connection, int skill, int target_id);

#endif //SERVER_GAME_H
