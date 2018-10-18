#include "game.h"

int character_number = 0;
extern MoveEventQueue *characterMoveEventQueue;
extern MoveEventQueue *aiMoveEventQueue;
extern NewcomerEventQueue *newcomerEventQueue;
extern ConnectionQueue *connectionQueue;
extern SkillEventQueue *skillEventQueue;
extern CreatureStateChangeEventQueue *creatureStateChangeEventQueue;
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
            characterMessages[i]->max_hp = conn->character->max_hp;
            characterMessages[i]->max_mp = conn->character->max_mp;
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
        creatureMessages[i]->exp = creatures[i].exp;
        creatureMessages[i]->state = creatures[i].state;
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
    if (!is_move_okay(y, x)) return -1;

    pthread_mutex_lock(&connection->character_data_lock);
    if (connection->character->hp <= 0) {
        pthread_mutex_unlock(&connection->character_data_lock);
        return -2;
    }
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
    if (map[connection->character->pos_y][connection->character->pos_x] == TERRAIN_ROSE)
        connection->character->hp = connection->character->hp + 5 > connection->character->max_hp ?
                                    connection->character->max_hp : connection->character->hp + 5;
    if (map[connection->character->pos_y][connection->character->pos_x] == TERRAIN_TULIP)
        connection->character->mp = connection->character->mp + 5 > connection->character->max_mp ?
                                    connection->character->max_mp : connection->character->mp + 5;
    pthread_mutex_unlock(&connection->character_data_lock);

    add_move_event(characterMoveEventQueue, connection->character->id, y, x);
    return 0;
}

float get_remaining_cd(Character ch, int sk) {
    struct timeval cur;
    gettimeofday(&cur, NULL);
    float remaining = SKILL_CD[ch.class][sk] -
                      cur.tv_sec + ch.last_cast[sk].tv_sec -
                      (float) (cur.tv_usec - ch.last_cast[sk].tv_usec) / 1000000;
    return remaining > 0 ? remaining : 0;
}

int use_skill(Connection *connection, int skill, int target_id) {
    int source_mp, target_hp;
    if (skill < 0 || skill > NUM_SKILLS_PER_CLASS)
        return 1;
    UnitType target_type = SKILL_TARGET_TYPE[connection->character->class][skill];
    pthread_mutex_lock(&connection->character_data_lock);
    if (connection->character->hp == 0) {
        pthread_mutex_unlock(&connection->character_data_lock);
        return 6; // dead character
    }
    float cd = get_remaining_cd(*connection->character, skill);
    if (cd > 0) {
        pthread_mutex_unlock(&connection->character_data_lock);
        return 2;
    }
    pthread_mutex_unlock(&connection->character_data_lock);
    if (target_type == UNIT_TYPE__CHARACTER) {
        Connection *conn = get_connection_by_id(connectionQueue, target_id);
        if (conn == NULL) return 3;
        if (get_euclidean_distance(conn->character->pos_y, conn->character->pos_x, connection->character->pos_y,
                                   connection->character->pos_x) > SKILL_RANGE[connection->character->class][skill])
            return 4;
        pthread_mutex_lock(&conn->character_data_lock);
        conn->character->hp -= SKILL_DAMAGE_RATE[connection->character->class][skill] * connection->character->ad;
        if (conn->character->hp > conn->character->max_hp) conn->character->hp = conn->character->max_hp;
        target_hp = conn->character->hp;
        pthread_mutex_unlock(&conn->character_data_lock);
    } else {
        if (get_euclidean_distance(creatures[target_id].pos_y, creatures[target_id].pos_x, connection->character->pos_y,
                                   connection->character->pos_x) > SKILL_RANGE[connection->character->class][skill])
            return 4;
        if (creatures[target_id].state == CREATURE_STATE__DEAD)
            return 5;
        pthread_mutex_lock(&creature_data_lock[target_id]);
        creatures[target_id].hp -= SKILL_DAMAGE_RATE[connection->character->class][skill] * connection->character->ad;
        if (creatures[target_id].hp <= 0) {
            creatures[target_id].state = CREATURE_STATE__DEAD;
            creatures[target_id].hp = 0;
            pthread_mutex_lock(&connection->character_data_lock);
            connection->character->exp += creatures[target_id].exp;
            while (connection->character->exp >= EXP_TO_UPGRADE[connection->character->level]) {
                connection->character->exp -= EXP_TO_UPGRADE[connection->character->level];
                connection->character->level++;
                connection->character->hp = connection->character->max_hp = CHARACTER_MAX_HP[connection->character->class][connection->character->level];
                connection->character->mp = connection->character->max_mp = CHARACTER_MAX_MP[connection->character->class][connection->character->level];
                connection->character->ad = CHARACTER_AD[connection->character->class][connection->character->level];
            }
            pthread_mutex_unlock(&connection->character_data_lock);
        } else {
            creatures[target_id].state = CREATURE_STATE__COMBAT;
            int is_threat = 0;
            for (int i = 0; i < (creatures[target_id].num_threats > MAX_NUM_THREATS ? MAX_NUM_THREATS
                                                                                    : creatures[target_id].num_threats); i++)
                if (creatures[target_id].threats[i] == connection->character->id) {
                    is_threat = 1;
                    break;
                }
            if (!is_threat) {
                creatures[target_id].num_threats++;
                creatures[target_id].threats[(creatures[target_id].num_threats - 1) %
                                             MAX_NUM_THREATS] = connection->character->id;
            }
        }
        CreatureState state = creatures[target_id].state;
        gettimeofday(&creatures[target_id].enter_state_time, NULL);
        target_hp = creatures[target_id].hp;
        pthread_mutex_unlock(&creature_data_lock[target_id]);
        add_creature_state_change_event(creatureStateChangeEventQueue, target_id, state);
    }
    pthread_mutex_lock(&connection->character_data_lock);
    connection->character->mp -= SKILL_MP_COST[connection->character->class][skill];
    source_mp = connection->character->mp;
    gettimeofday(&connection->character->last_cast[skill], NULL);
    pthread_mutex_unlock(&connection->character_data_lock);
    add_skill_event(skillEventQueue, UNIT_TYPE__CHARACTER, connection->character->id, target_type, target_id, skill,
                    source_mp, target_hp);
    return 0;
};