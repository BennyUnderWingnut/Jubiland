#include "server.h"

void init_server(int port, int threads, int max_jobs, int max_connections);

void *listen_port();

int sock, running, wakeup_count;
//ThreadPool *threadPool;
threadpool thpool;
ConnectionQueue *connectionQueue;
MoveEventQueue *characterMoveEventQueue;
MoveEventQueue *aiMoveEventQueue;
NewcomerEventQueue *newcomerEventQueue;
LogoutEventQueue *logoutEventQueue;
SkillEventQueue *skillEventQueue;
CreatureStateChangeEventQueue *creatureStateChangeEventQueue;
pthread_mutex_t ai_move_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t remove_not_responding_connections_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int ac, char *av[]) {
    int threads = DEFAULT_NUM_THREADS;
    int jobs = DEFAULT_MAX_NUM_JOBS;
    int connections = DEFAULT_MAX_NUM_PLAYERS;
    if (ac == 1) {
        fprintf(stderr, "usage: tws portnum\n");
        exit(1);
    }
    int port = (int) strtol(av[1], NULL, 10);
    if (ac >= 3)
        threads = (int) strtol(av[2], NULL, 10);
    if (ac >= 4)
        jobs = (int) strtol(av[3], NULL, 10);
    if (ac >= 5)
        connections = (int) strtol(av[3], NULL, 10);
    init_server(port, threads, jobs, connections);
}

void stop_listening() {
    running = 0;
    close(sock);
}

void *handle_call(void *fdptr) {
    int rv, rm_conn = 0, fd = *(int *) fdptr;
    Connection *conn = connection_queue_get_connection(connectionQueue, fd);
    if (conn == NULL) {
        fprintf(stderr, "Cannot get _Connection\n");
        return NULL;
    }
    Request *req;
    int *stat = malloc(sizeof(int));
    req = get_request(fd, stat);
    if (req == NULL) {
        rm_conn = 1;
        if (*stat != 1)
            fprintf(stderr, "Error unpacking incoming message\n");
    } else {
        switch (req->type) {
            case REQUEST__TYPE__LOGIN:
                if (req->login == NULL) break;
                rv = add_character(connectionQueue, conn, req->login->class_, req->login->nickname);
                if (rv) {
                    printf("Failed login, Connection removed.");
                    send_login_fail(conn, rv);
                    rm_conn = 1;
                } else {
                    printf("New player logged in: %s\n", req->login->nickname);
                    send_welcome_message(conn, conn->key);
                    send_world_status(connectionQueue, conn);
                }
                break;
            case REQUEST__TYPE__MOVE:
                if (req->move == NULL) break;
                if (req->key != conn->key) break;
                move_character(conn, req->move->pos_y, req->move->pos_x);
                break;
            case REQUEST__TYPE__SKILL:
                if (req->skill == NULL) break;
                if (req->key != conn->key) break;
                use_skill(conn, req->skill->skill, req->skill->target_id);
                break;
            case REQUEST__TYPE__KEEP:
                if (req->key != conn->key) break;
                gettimeofday(&conn->last_keep_connection, NULL);
                reset_key(conn);
                break;
            default:;
        }
        request__free_unpacked(req, NULL);
    }
    conn->listened = 0;
    if (rm_conn)
        connection_queue_remove_connection(connectionQueue, conn);
    free(stat);
    return NULL;
}

void *listen_port() {
    int fd, rv, max_fd;
    Connection *conn, *next;
    fd_set set;
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    signal(SIGINT, stop_listening);
    signal(SIGTERM, stop_listening);
    signal(SIGPIPE, handle_sigpipe);
    while (running) {
        max_fd = sock;
        FD_ZERO(&set);
        FD_SET(sock, &set);
        conn = connectionQueue->head->next;
        while (conn != NULL) {
            if (!conn->listened) {
                FD_SET(conn->fd, &set);
                if (conn->fd > max_fd) max_fd = conn->fd;
            }
            conn = conn->next;
        }
        rv = select(max_fd + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            perror("Failed to select: ");
        } else if (rv == 0) {
            // DO NOTHING
        } else {
            conn = connectionQueue->head->next;
            if (FD_ISSET(sock, &set)) {
                fd = accept(sock, NULL, NULL);
                connection_queue_add_connection(connectionQueue, fd);
                printf("Connection established\n");
            }
            while (conn != NULL) {
                next = conn->next;
                if (FD_ISSET(conn->fd, &set)) {
                    conn->listened = 1;
                    handle_call(&conn->fd);
                    //create_detached_thread(handle_call, &conn->fd);
                    //thread_pool_add_job(threadPool, handle_call, &conn->fd);
                    //thpool_add_work(thpool, (void *) handle_call, &conn->fd);
                }
                conn = next;
            }
        }
    }
    return NULL;
}

void broadcast_events() {
    wakeup_count = (wakeup_count + 1) % 100000;
    pthread_mutex_unlock(&ai_move_lock);
    pthread_mutex_unlock(&remove_not_responding_connections_lock);

    int i, moves, aimoves, newcomers, logouts, skills, cschanges;
    Connection *conn = connectionQueue->head->next;

    MoveEvent *me = pop_move_events(characterMoveEventQueue, &moves), *pme;
    MoveEvent *ame = pop_move_events(aiMoveEventQueue, &aimoves), *pame;
    NewcomerEvent *ne = pop_newcomer_events(newcomerEventQueue, &newcomers), *pne;
    LogoutEvent *le = pop_logout_events(logoutEventQueue, &logouts), *ple;
    SkillEvent *se = pop_skill_events(skillEventQueue, &skills), *pse;
    CreatureStateChangeEvent *ce = pop_creature_state_change_events(creatureStateChangeEventQueue, &cschanges), *pce;

    if (moves == 0 && aimoves == 0 && newcomers == 0 && logouts == 0 && skills == 0 && cschanges == 0) return;
    Response resp = RESPONSE__INIT;
    EventsMessage em = EVENTS_MESSAGE__INIT;
    resp.events = &em;
    resp.type = RESPONSE__TYPE__EVENTS;

    em.n_moves = (size_t) moves;
    em.n_aimoves = (size_t) aimoves;
    em.n_newcomers = (size_t) newcomers;
    em.n_logouts = (size_t) logouts;
    em.n_skills = (size_t) skills;
    em.n_cschanges = (size_t) cschanges;

    if (moves != 0) {
        em.moves = malloc(sizeof(MoveMessage *) * moves);
        for (i = 0; i < moves; i++) {
            em.moves[i] = malloc(sizeof(MoveMessage));
            move_message__init(em.moves[i]);
            em.moves[i]->id = me->id;
            em.moves[i]->pos_y = me->pos_y;
            em.moves[i]->pos_x = me->pos_x;
            pme = me;
            me = me->next;
            free(pme);
        }
    }
    if (aimoves != 0) {
        em.aimoves = malloc(sizeof(MoveMessage *) * aimoves);
        for (i = 0; i < aimoves; i++) {
            em.aimoves[i] = malloc(sizeof(MoveMessage));
            move_message__init(em.aimoves[i]);
            em.aimoves[i]->id = ame->id;
            em.aimoves[i]->pos_y = ame->pos_y;
            em.aimoves[i]->pos_x = ame->pos_x;
            pame = ame;
            ame = ame->next;
            free(pame);
        }
    }
    if (newcomers != 0) {
        em.newcomers = malloc(sizeof(CharacterMessage *) * newcomers);
        for (i = 0; i < newcomers; i++) {
            em.newcomers[i] = malloc(sizeof(CharacterMessage));
            character_message__init(em.newcomers[i]);
            em.newcomers[i]->nickname = ne->nickname;
            em.newcomers[i]->class_ = ne->class;
            em.newcomers[i]->level = ne->level;
            em.newcomers[i]->pos_y = ne->pos_y;
            em.newcomers[i]->pos_x = ne->pos_x;
            em.newcomers[i]->hp = ne->hp;
            em.newcomers[i]->mp = ne->mp;
            em.newcomers[i]->id = ne->id;
            em.newcomers[i]->exp = ne->exp;
            pne = ne;
            ne = ne->next;
            free(pne);
        }
    }
    if (logouts != 0) {
        em.logouts = malloc(sizeof(LogoutMessage *) * logouts);
        for (i = 0; i < logouts; i++) {
            em.logouts[i] = malloc(sizeof(LogoutMessage));
            logout_message__init(em.logouts[i]);
            em.logouts[i]->id = le->id;
            ple = le;
            le = le->next;
            free(ple);
        }
    }
    if (skills != 0) {
        em.skills = malloc(sizeof(SkillMessage *) * skills);
        for (i = 0; i < skills; i++) {
            em.skills[i] = malloc(sizeof(SkillMessage));
            skill_message__init(em.skills[i]);
            em.skills[i]->target_type = se->target_type;
            em.skills[i]->target_id = se->target_id;
            em.skills[i]->source_type = se->source_type;
            em.skills[i]->source_id = se->source_id;
            em.skills[i]->skill = se->skill;
            em.skills[i]->source_mp = se->source_mp;
            em.skills[i]->target_hp = se->target_hp;
            pse = se;
            se = se->next;
            free(pse);
        }
    }
    if (cschanges != 0) {
        em.cschanges = malloc(sizeof(CreatureStateChangeMessage *) * cschanges);
        for (i = 0; i < cschanges; i++) {
            em.cschanges[i] = malloc(sizeof(CreatureStateChangeMessage));
            creature_state_change_message__init(em.cschanges[i]);
            em.cschanges[i]->id = ce->id;
            em.cschanges[i]->state = ce->state;
            pce = ce;
            ce = ce->next;
            free(pce);
        }
    }

    while (conn != NULL) {
        send_response(conn->fd, resp);
        conn = conn->next;
    }

    for (i = 0; i < moves; i++)
        free(em.moves[i]);
    if (moves > 0)
        free(em.moves);

    for (i = 0; i < aimoves; i++)
        free(em.aimoves[i]);
    if (aimoves > 0)
        free(em.aimoves);

    for (i = 0; i < newcomers; i++)
        free(em.newcomers[i]);
    if (newcomers > 0)
        free(em.newcomers);

    for (i = 0; i < logouts; i++)
        free(em.logouts[i]);
    if (logouts > 0)
        free(em.logouts);

    for (i = 0; i < skills; i++)
        free(em.skills[i]);
    if (skills > 0)
        free(em.skills);

    for (i = 0; i < cschanges; i++)
        free(em.cschanges[i]);
    if (cschanges > 0)
        free(em.cschanges);
}

void *begin_broadcast() {
    sigset_t sigset;
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_UNBLOCK, &sigset, NULL); // unblock alarm signal
    signal(SIGALRM, broadcast_events);
    set_ticker(100);
    while (running) {
        sleep(1);
    }
    return NULL;
}

void init_server(int port, int threads, int max_jobs, int max_connections) {
    running = 1;
    init_map();
    init_creatures();
    init_characters();

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL); // block alarm signal

    sock = make_server_socket(port);
    if (sock == -1) exit(2);
    //threadPool = thread_pool_init(threads, max_jobs);
    thpool = thpool_init(threads);
    connectionQueue = connection_queue_init(max_connections);
    characterMoveEventQueue = move_event_queue_init();
    aiMoveEventQueue = move_event_queue_init();
    newcomerEventQueue = newcomer_event_queue_init();
    logoutEventQueue = logout_event_queue_init();
    skillEventQueue = skill_event_queue_init();
    creatureStateChangeEventQueue = creature_state_change_event_queue_init();
    create_detached_thread(begin_broadcast, NULL);
    create_detached_thread(ai_loop, NULL);
    create_detached_thread(remove_not_responding_connections_loop, NULL);
    listen_port();
    /* * create a thread to listen port
     * pthread_t t;
     * pthread_attr_t attr_detached;
     * pthread_attr_init(&attr_detached);
     * pthread_attr_setdetachstate(&attr_detached, PTHREAD_CREATE_DETACHED);
     * pthread_create(&t, &attr_detached, listen_port, NULL);
     * */
}