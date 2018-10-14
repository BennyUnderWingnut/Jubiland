#ifndef CLIENT_SYNC_H
#define CLIENT_SYNC_H

#include "mylib.h"
#include "map.h"
#include "login.h"
#include "socklib.h"
#include "locale.h"

void init_world(int id);

void sync_move(void);

#endif //CLIENT_SYNC_H
