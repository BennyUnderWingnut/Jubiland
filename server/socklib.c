#include "socklib.h"

#define HOSTLEN 256
#define BACKLOG 1

int make_server_socket_q(int, int);


/**
 * returns a server socket or -1 if error
 */
int make_server_socket(int portnum) {
    return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog) {
    struct sockaddr_in saddr; /* build our address here */
    struct hostent *hp; /* this is part of our */
    char hostname[HOSTLEN]; /* address */
    int sock_id; /* the socket*/

    printf("Creating socket...\n");
    sock_id = socket(PF_INET, SOCK_STREAM, 0); /* get a socket */
    if (sock_id == -1) {
        perror("Failed to get socket: ");
        return -1;
    } else printf("Socket created.\n");

/** build address and bind it to socket **/
    bzero((void *) &saddr, sizeof(saddr)); /* clear out struct */
    gethostname(hostname, HOSTLEN); /* where am I ?  */
    printf("Host name is %s\n", hostname);
    hp = gethostbyname(hostname);/* get info about host */
    /* fill in host part */
    bcopy((void *) hp->h_addr, (void *) &saddr.sin_addr, hp->h_length); /* fill in internet address */
    saddr.sin_port = htons(portnum); /* fill in socket port */
    saddr.sin_family = AF_INET; /* fill in address family */
    printf("Binding socket...\n");
    if (bind(sock_id, (struct sockaddr *) &saddr, sizeof(saddr)) != 0) {
        perror("Failed to bind socket: ");
        return -1;
    } else printf("Socket bound\n");
/** arrange for incoming calls **/
    printf("Listening for incoming connections...\n");
    if (listen(sock_id, backlog) != 0) {
        perror("Failed to listen for incoming connections: ");
        return -1;
    }
    return sock_id;
}

Request *get_request(int fd, int *stat) {
    *stat = 0;
    char ch[1];
    uint8_t buf[BUFF_SIZE], msg[MAX_MSG_SIZE];
    size_t i = 0;
    char size[20];
    long received = 0, rv;
    while ((rv = recv(fd, ch, 1, 0)) != 0) {
        if (ch[0] == '\n') break;
        size[i++] = ch[0];
    }
    if (rv < 0) {
        perror("Error happened when receiving message: ");
        return NULL;
    }
    if (rv == 0) {
        printf("Socket %d closed by client\n", fd);
        *stat = 1;
        return NULL;
    }
    size[i] = '\0';
    long msg_size = strtol(size, NULL, 10);
    printf("Message size = %ld bytes\n", msg_size);
    if (msg_size <= 0) {
        fprintf(stderr, "Illegal message length\n");
        return NULL;
    }
    if (msg_size > MAX_MSG_SIZE) {
        fprintf(stderr, "Message too large\n");
        return NULL;
    }
    while (received < msg_size) {
        if (msg_size - received > BUFF_SIZE) rv = recv(fd, buf, BUFF_SIZE, 0);
        else rv = recv(fd, buf, (size_t) (msg_size - received), 0);
        memcpy(msg + received, buf, BUFF_SIZE);
        received += rv;
        printf("%ld bytes received from %d.\n", received, fd);
    }
    Request *req = request__unpack(NULL, (size_t) msg_size, msg);
    return req;
}

int send_response(int fd, Response resp) {
    void *buf;
    size_t len; // Length of serialized data
    char size_buf[10];
    len = response__get_packed_size(&resp);
    buf = malloc(len);
    response__pack(&resp, buf);
    sprintf(size_buf, "%ld\n", len);
    send(fd, size_buf, strlen(size_buf), 0);
    send(fd, buf, len, 0);
    free(buf);
    return 0;
}

void handle_sigpipe() {
    printf("A client closed socket\n");
}