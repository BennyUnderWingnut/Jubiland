#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include "interface.h"
#include "sync.h"

void main_loop(void);

void *listen_events(void *sockptr);

void end_game();

#endif //CLIENT_GAME_H
