#ifndef CLIENT_SYNC_H
#define CLIENT_SYNC_H

#include "mylib.h"
#include "login.h"
#include "socklib.h"
#include "creature.h"

void init_world(int id);

void sync_move(void);

void sync_skill(int skill);

#endif //CLIENT_SYNC_H
