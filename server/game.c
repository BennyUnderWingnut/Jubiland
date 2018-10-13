#include "game.h"

int character_number = 0;
pthread_mutex_t character_number_lock;

int add_character(ConnectionQueue *connectionQueue, Connection *connection, int class, char *nickname) {
    Connection *conn = connectionQueue->head->next;
    while (conn != NULL) {
        if (conn->character != NULL && !strcmp(conn->character->nickname, nickname))
            return 1; // Nickname taken
        conn = conn->next;
    }
    if (class < 0 || class >= CLASS_TYPES)
        return 2; // Wrong class
    Character *character;
    if ((character = init_character(class, nickname)) == NULL) {
        return 3; // Unknown error
    }
    pthread_mutex_lock(&character_number_lock);
    connection->character = character;
    character_number++;
    pthread_mutex_unlock(&character_number_lock);
    return 0;
}

int remove_character(Connection *conn) {
    pthread_mutex_lock(&conn->character_data_lock);
    free(conn->character);
    pthread_mutex_unlock(&conn->character_data_lock);
    return 0;
}

int send_world_status(ConnectionQueue *queue, Connection *connection) {
    int i = 0;
    Connection *conn = queue->head->next;
    Response resp = RESPONSE__INIT;
    WorldStateMessage ws = WORLD_STATE_MESSAGE__INIT;
    resp.type = RESPONSE__REQUEST_TYPE__WORLD_STATE;
    resp.worldstate = &ws;
    pthread_mutex_lock(&queue->queue_lock);
    pthread_mutex_lock(&character_number_lock);
    CharacterMessage **characterMessages = malloc(sizeof(CharacterMessage *) * character_number);
    ws.n_charters = (size_t) character_number;
    pthread_mutex_unlock(&character_number_lock);
    while (conn != NULL) {
        pthread_mutex_lock(&conn->character_data_lock);
        if (conn->character != NULL) {
            characterMessages[i] = malloc(sizeof(CharacterMessage));
            character_message__init(characterMessages[i]);
            characterMessages[i]->nickname = conn->character->nickname;
            characterMessages[i]->id = conn->character->id;
            characterMessages[i]->class_ = (CharacterMessage__CharacterClass) conn->character->class;
            characterMessages[i]->exp = conn->character->exp;
            characterMessages[i]->level = conn->character->level;
            characterMessages[i]->pos_x = conn->character->pos_x;
            characterMessages[i]->pos_y = conn->character->pos_y;
            characterMessages[i]->hp = conn->character->hp;
            characterMessages[i]->mp = conn->character->mp;
            i++;
        }
        pthread_mutex_unlock(&conn->character_data_lock);
        conn = conn->next;
    }
    pthread_mutex_unlock(&queue->queue_lock);
    ws.charters = characterMessages;
    send_response(connection->fd, resp);
    for (i = 0; i < character_number; i++) {
        free(characterMessages[i]);
    }
    free(characterMessages);
    return 0;
}