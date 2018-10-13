#include "socklib.h"

int connect_to_server(char *host, int port) {
    int sock;
    struct sockaddr_in server_address; /* the number to call */
    struct hostent *hp; /* used to get number */
/** Step 1: Get a socket **/
    sock = socket(AF_INET, SOCK_STREAM, 0); /* get a line */
    if (sock == -1) {
        return -1;
    }
/** Step 2: connect to server **/
    bzero(&server_address, sizeof(server_address)); /* zero the address*/
    hp = gethostbyname(host); /* lookup host's ip # */
    if (hp == NULL) {
        return -1;
    }
    bcopy(hp->h_addr, (struct sockaddr *) &server_address.sin_addr, hp->h_length); /* fill in internet address */
    server_address.sin_port = htons(port); /* fill in port number */
    server_address.sin_family = AF_INET; /* fill in socket type */
    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
        return -1;
    }
    return sock;
}


Response *get_response(int fd) {
    uint8_t buf[MAX_MSG_SIZE], msg[MAX_MSG_SIZE];
    char ch[1];
    size_t i = 0;
    char size[20];
    while (recv(fd, ch, 1, 0) != 0) {
        if (ch[0] == '\n') break;
        size[i++] = ch[0];
    }
    size[i] = '\0';
    long msg_size = strtol(size, NULL, 10);
    long received = 0, rv;
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
    }
    Response *resp = response__unpack(NULL, (size_t) msg_size, msg);
    return resp;
}

int send_request(int sock, Request req) {
    void *buf; // Buffer to store serialized data
    size_t len; // Length of serialized data
    char size_buf[100];
    len = request__get_packed_size(&req);
    buf = malloc(len);
    request__pack(&req, buf);
    sprintf(size_buf, "%ld\n", len);
    send(sock, size_buf, strlen(size_buf), 0);
    send(sock, buf, len, 0);
    free(buf); // Free the allocated serialized buffer
    return 0;
}