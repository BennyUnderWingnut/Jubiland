#include "game.h"

int character_number = 0;
extern MoveEventQueue *characterMoveEventQueue;
extern MoveEventQueue *aiMoveEventQueue;
extern NewcomerEventQueue *newcomerEventQueue;
Creature creatures[CREATURE_NUM];
pthread_mutex_t creature_data_lock[CREATURE_NUM];

int add_character(ConnectionQueue *connectionQueue, Connection *connection, int class, char *nickname) {
    pthread_mutex_lock(&connectionQueue->queue_lock);
    Connection *conn = connectionQueue->head->next;
    while (conn != NULL) {
        if (conn->character != NULL && !strcmp(conn->character->nickname, nickname)) {
            pthread_mutex_unlock(&connectionQueue->queue_lock);
            return 1; // Nickname taken
        }
        conn = conn->next;
    }
    if (class < 0 || class >= CHARACTER_CLASS_TYPES) {
        pthread_mutex_unlock(&connectionQueue->queue_lock);
        return 2; // Wrong class
    }
    Character *character;
    if ((character = init_character(class, nickname)) == NULL) {
        pthread_mutex_unlock(&connectionQueue->queue_lock);
        return 3; // Unknown error
    }
    connection->character = character;
    character_number++;
    pthread_mutex_unlock(&connectionQueue->queue_lock);
    add_newcomer_event(newcomerEventQueue, *connection->character);
    return 0;
}

int send_world_status(ConnectionQueue *queue, Connection *connection) {
    int i = 0;
    Connection *conn = queue->head->next;
    Response resp = RESPONSE__INIT;
    WorldStateMessage ws = WORLD_STATE_MESSAGE__INIT;
    resp.type = RESPONSE__TYPE__WORLD_STATE;
    resp.worldstate = &ws;

    pthread_mutex_lock(&queue->queue_lock);
    ws.n_charters = (size_t) character_number;
    CharacterMessage **characterMessages = malloc(sizeof(CharacterMessage *) * ws.n_charters);
    ws.charters = characterMessages;
    while (conn != NULL) {
        pthread_mutex_lock(&conn->character_data_lock);
        if (conn->character != NULL) {
            characterMessages[i] = malloc(sizeof(CharacterMessage));
            character_message__init(characterMessages[i]);
            characterMessages[i]->nickname = conn->character->nickname;
            characterMessages[i]->id = conn->character->id;
            characterMessages[i]->class_ = conn->character->class;
            characterMessages[i]->exp = conn->character->exp;
            characterMessages[i]->level = conn->character->level;
            characterMessages[i]->pos_x = conn->character->pos_x;
            characterMessages[i]->pos_y = conn->character->pos_y;
            characterMessages[i]->hp = conn->character->hp;
            characterMessages[i]->mp = conn->character->mp;
            characterMessages[i]->ad = conn->character->ad;
            i++;
        }
        pthread_mutex_unlock(&conn->character_data_lock);
        conn = conn->next;
    }
    pthread_mutex_unlock(&queue->queue_lock);

    ws.n_creatures = CREATURE_NUM;
    CreatureMessage **creatureMessages = malloc(sizeof(CreatureMessage *) * ws.n_creatures);
    ws.creatures = creatureMessages;
    for (i = 0; i < ws.n_creatures; i++) {
        pthread_mutex_lock(&creature_data_lock[i]);
        creatureMessages[i] = malloc(sizeof(CreatureMessage));
        creature_message__init(creatureMessages[i]);
        creatureMessages[i]->max_hp = creatures[i].max_hp;
        creatureMessages[i]->level = creatures[i].level;
        creatureMessages[i]->pos_y = creatures[i].pos_y;
        creatureMessages[i]->pos_x = creatures[i].pos_x;
        creatureMessages[i]->category = creatures[i].category;
        creatureMessages[i]->id = creatures[i].id;
        creatureMessages[i]->hp = creatures[i].hp;
        creatureMessages[i]->ad = creatures[i].ad;
        pthread_mutex_unlock(&creature_data_lock[i]);
    }

    send_response(connection->fd, resp);

    for (i = 0; i < character_number; i++) {
        free(characterMessages[i]);
    }
    free(creatureMessages);
    for (i = 0; i < CREATURE_NUM; i++) {
        free(creatureMessages[i]);
    }
    free(characterMessages);
    return 0;
}

int move_character(Connection *connection, int y, int x) {
    // Check is move ok
    if (y < 0 || y > MAP_LINES - 1 || x < 0 || x > MAP_COLS - 1 || map[y][x] != TERRAIN_GRASS) return -1;

    pthread_mutex_lock(&connection->character_data_lock);
    struct timeval last_move = connection->character->last_move;
    struct timeval this_move;
    gettimeofday(&this_move, NULL);
    int delta = (int) ((this_move.tv_sec - last_move.tv_sec) * 1000000 + this_move.tv_usec - last_move.tv_usec);
    int accumulated_delta = MOVE_INTERVAL_USEC - delta + connection->character->move_interval_buffer;
    if (accumulated_delta > MOVE_INTERVAL_MAX_BUFFER_USEC) {
        pthread_mutex_unlock(&connection->character_data_lock);
        return -1;
    }
    if (accumulated_delta < -MOVE_INTERVAL_MAX_BUFFER_USEC) accumulated_delta = -MOVE_INTERVAL_MAX_BUFFER_USEC;
    connection->character->move_interval_buffer = accumulated_delta;

    connection->character->pos_y = y;
    connection->character->pos_x = x;
    pthread_mutex_unlock(&connection->character_data_lock);

    add_move_event(characterMoveEventQueue, connection->character->id, y, x);
    return 0;
}