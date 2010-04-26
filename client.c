//#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "communicator.h"
#include <stdio.h>


void *client_thd(void *arg) {
  fprintf(stderr, "Starting client thread");
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
    serverInfo->protocol = portno;
    communicate(serverInfo);
}

void client_handler(int status, void *data) {
  puts("client handler");
  if (status == HANDLE_NEW_CONNECTION) {
    fprintf(stderr, "client handle new connection\n");
    struct RemoteConnection *remote = (struct RemoteConnection *) data;
    char buf[100];
    while (1) {
      scanf("%s", buf);
      sendData(remote, buf, strlen(buf) + 1);
    }
  }
  if (status == HANDLE_NEW_DATA) {
    struct IncomingData *indata = (struct IncomingData *) data;
    printf("Incoming message from fd=%d, port=%d\n", indata->remoteConnection.comm_sock_fd, indata->remoteConnection.protocol);
    puts((char *) indata->data);
  }
}

void start_client() {
  puts("Enter host:");
  char buf[50];
  char hostName[50];
  scanf("%s", hostName);
  //strcpy(buf, hostName);
  puts("Enter port:");
  int port;
  scanf("%s", buf);
  port = atoi(buf);
  setHandler(&client_handler);
  pthread_join(*issueCommunicationThread(CLIENT, hostName, port), NULL);
}


