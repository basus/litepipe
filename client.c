//#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "communicator.h"



void *client_thd(void *arg) {
    struct ServerAddress *addr = (struct ServerAddress *) arg;
    connect_to_server(addr->host, addr->port);
}

void connect_to_server(char *serverName, int portno) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(serverName);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,
           (char *) &serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    struct RemoteConnection *serverInfo = (struct RemoteConnection *) malloc(sizeof(struct RemoteConnection));
    serverInfo->comm_sock_fd = sockfd;
    serverInfo->client_addr = &serv_addr;
    communicate(serverInfo);
}

