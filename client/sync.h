#ifndef CLIENT_SYNC_H
#define CLIENT_SYNC_H

#include "mylib.h"
#include "login.h"
#include "socklib.h"
#include "creature.h"

void init_world(int id);

void sync_move(void);

int sync_skill(int skill);

void send_keep_connection_request();

#endif //CLIENT_SYNC_H
