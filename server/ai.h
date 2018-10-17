#ifndef SERVER_AI_H
#define SERVER_AI_H

#include "mylib.h"
#include "creature.h"
#include "event.h"

void init_creatures(void);

void *ai_loop();

#endif //SERVER_AI_H
