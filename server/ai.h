#ifndef SERVER_AI_H
#define SERVER_AI_H

#include "mylib.h"
#include "creature.h"
#include "event.h"
#include "connection.h"
#include "astar.h"

#define THREAT_LOSE_SEC 20

void init_creatures(void);

void *ai_loop();

#endif //SERVER_AI_H
