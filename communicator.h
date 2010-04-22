
#ifndef __COMMUNICATOR_H__
#define __COMMUNICATOR_H__

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>

#include "server.h"
#include "client.h"

#define SERVER 0
#define CLIENT 1

//data will be RemoteConnection struct
#define HANDLE_NEW_CONNECTION 0
//data will be IncomingData struct
#define HANDLE_NEW_DATA 1
//data will be the RemoteConnection struct
#define HANDLE_CONNECTION_BROKEN 2


#define COMMUNICATOR_BUF_SIZE 256
struct RemoteConnection {
    int comm_sock_fd;
        int protocol;
    struct sockaddr_in *client_addr;
};

struct IncomingData {
    struct RemoteConnection remoteConnection;
    char data[COMMUNICATOR_BUF_SIZE];
    int ndata;
};




///to be used for client_thd in client.c
struct ServerAddress {
    char *host;
    int port;
};

void setHandler(void (*)(int status, void *data));

void communicate(struct RemoteConnection *);

//int send_msg_str(char *, int);
void sendData(struct RemoteConnection *, void *data, int ndata);

//host is ignored when type == SERVER
void issueCommunicationThread(int type, char *host, int port);

void error(char *msg);

#endif
