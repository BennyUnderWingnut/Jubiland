#include "server.h"

void init_server(int port, int threads, int max_jobs, int max_connections);

void *listen_port();

time_t server_started;
int sock, running;
//ThreadPool *threadPool;
threadpool thpool;
ConnectionQueue *connectionQueue;
MoveEventQueue *moveEventQueue;
NewcomerEventQueue *newcomerEventQueue;

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
    printf("fd is %d\n", fd);
    int *stat = malloc(sizeof(int));
    req = get_request(fd, stat);
    if (req == NULL) {
        rm_conn = 1;
        if (*stat != 1)
            fprintf(stderr, "Error unpacking incoming message\n");
    } else {
        time(&conn->last_request);
        switch (req->type) {
            case REQUEST_TYPE__LOGIN:
                if (req->login == NULL) break;
                rv = add_character(connectionQueue, conn, req->login->class_, req->login->nickname);
                if (rv) {
                    printf("Failed login, _Connection removed.");
                    send_login_fail(conn, rv);
                    rm_conn = 1;
                } else {
                    printf("New player logged in: %s\n", req->login->nickname);
                    send_welcome_message(conn, conn->key);
                    send_world_status(connectionQueue, conn);
                }
                break;
            case REQUEST_TYPE__MOVE:
                if (req->move == NULL) break;
                if (req->move->key != conn->key) break;
                move_character(conn, req->move->pos_y, req->move->pos_x);
                break;
            default:;
        }
        request__free_unpacked(req, NULL);
    }
    conn->listened = 0;
    if (rm_conn) connection_queue_remove_connection(connectionQueue, conn);
    free(stat);
    return NULL;
}

void *listen_port() {
    running = 1;
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
            printf("Timeout\n");
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
    int moves, newcomers, i;
    Connection *conn = connectionQueue->head->next;
    MoveEvent *me = pop_move_events(moveEventQueue, &moves);
    NewcomerEvent *ne = pop_newcomer_events(newcomerEventQueue, &newcomers);
    if (moves == 0 && newcomers == 0) return;
    Response resp = RESPONSE__INIT;
    EventsMessage em = EVENTS_MESSAGE__INIT;
    resp.events = &em;
    resp.type = RESPONSE__REQUEST_TYPE__EVENTS;

    em.n_moveevents = (size_t) moves;
    em.n_newcomerevents = (size_t) newcomers;

    if (moves != 0) {
        em.moveevents = malloc(sizeof(MoveEventMessage *) * moves);
        for (i = 0; i < moves; i++) {
            em.moveevents[i] = malloc(sizeof(MoveEventMessage));
            move_event_message__init(em.moveevents[i]);
            em.moveevents[i]->id = me->id;
            em.moveevents[i]->pos_y = me->pos_y;
            em.moveevents[i]->pos_x = me->pos_x;
            me = me->next;
        }
    }
    if (newcomers != 0) {
        em.newcomerevents = malloc(sizeof(NewcomerEventMessage *) * newcomers);
        for (i = 0; i < newcomers; i++) {
            em.newcomerevents[i] = malloc(sizeof(NewcomerEventMessage));
            newcomer_event_message__init(em.newcomerevents[i]);
            em.newcomerevents[i]->nickname = ne->nickname;
            em.newcomerevents[i]->class_ = (NewcomerEventMessage__CharacterClass) ne->class;
            em.newcomerevents[i]->level = ne->level;
            em.newcomerevents[i]->pos_y = ne->pos_y;
            em.newcomerevents[i]->pos_x = ne->pos_x;
            em.newcomerevents[i]->hp = ne->hp;
            em.newcomerevents[i]->mp = ne->mp;
            em.newcomerevents[i]->id = ne->id;
            em.newcomerevents[i]->exp = ne->exp;
        }
    }
    while (conn != NULL) {
        send_response(conn->fd, resp);
        conn = conn->next;
    }
    for (i = 0; i < moves; i++)
        free(em.moveevents[i]);
    free(em.moveevents);
}

void *begin_broadcast() {
    sigset_t sigset;
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_UNBLOCK, &sigset, NULL); // unblock alarm signal
    signal(SIGALRM, broadcast_events);
    set_ticker(10);
    while (running) {
        sleep(1);
    }
    return NULL;
}

void init_server(int port, int threads, int max_jobs, int max_connections) {
    init_map();

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL); // block alarm signal

    sock = make_server_socket(port);
    if (sock == -1) exit(2);
    time(&server_started);
    //threadPool = thread_pool_init(threads, max_jobs);
    thpool = thpool_init(threads);
    connectionQueue = connection_queue_init(max_connections);
    moveEventQueue = move_event_queue_init();
    newcomerEventQueue = newcomer_event_queue_init();
    create_detached_thread(begin_broadcast, NULL);
    listen_port();
    /* * create a thread to listen port
     * pthread_t t;
     * pthread_attr_t attr_detached;
     * pthread_attr_init(&attr_detached);
     * pthread_attr_setdetachstate(&attr_detached, PTHREAD_CREATE_DETACHED);
     * pthread_create(&t, &attr_detached, listen_port, NULL);
     * */
}