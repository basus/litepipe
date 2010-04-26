
//#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "communicator.h"
#include "server.h"
#include "client.h"




//void send_msg(int);

void *recv_msg(void *);

void (*handle)(int, void *);

void communicate(struct RemoteConnection *clientInfo) {
    
    pthread_t *receive_thread = (pthread_t *) malloc(sizeof(pthread_t));
    int ret = pthread_create(receive_thread, NULL, recv_msg, clientInfo);
    if (ret)
      error("Cannot create thread.");
    handle(HANDLE_NEW_CONNECTION, clientInfo);
}

void sendData(struct RemoteConnection *remoteInfo, void *data, int ndata) {
  fprintf(stderr, "Sending data on fd=%d ndata=%d \ndata=%s\n", remoteInfo->comm_sock_fd, ndata, data == NULL ? "NULL" : (char *) data);
    int n = send(remoteInfo->comm_sock_fd, &ndata, 4, 0);
    if (n < 0)
        error("ERROR writing to socket");
    n = send(remoteInfo->comm_sock_fd, data, ndata, 0);
    
    if (n < 0)
        error("ERROR writing to socket");
}

void setHandler(void (*fun)(int, void *)) {
    handle = fun;
}


void *recv_msg(void *arg) {
    struct RemoteConnection *clientInfo = (struct ClientInfo *) arg;
    int fd = clientInfo->comm_sock_fd;
    const char buf[COMMUNICATOR_BUF_SIZE];
    bzero(buf, COMMUNICATOR_BUF_SIZE);
    struct pollfd fds[1];
    fds->fd = fd;
    fds->events = POLLIN;
    do {
        bzero(buf, strlen(buf));
        /*
        poll(fds, 1, -1);
        if (fds->revents & (POLLERR | POLLHUP | POLLNVAL)) {
             puts("Connection broken by peer.");
             return;
        }
        */
	
	//first read the number of bytes expected.
	unsigned int transmission_len;
	int read_len = recv(fd, &transmission_len, 4, 0);
	if (read_len < 0)
            error("ERROR reading from socket");
        if (read_len == 0) {
            fprintf(stderr, "Empty message received, breaking. (fd=%d)\n", fd);
            break;
        }
	if (read_len != 4) 
	  error("Protocol error: message too short");

	//create an IncomingData struct
	struct IncomingData *data = (struct IncomingData *) malloc(sizeof(struct IncomingData));
        data->remoteConnection = *clientInfo;
        data->data = malloc(transmission_len);
        data->ndata = transmission_len;

	for (int i = 0; i < transmission_len; i += read_len) {
	  read_len = recv(fd, data->data + i, transmission_len - i, 0);
	  if (read_len < 0)
            error("ERROR reading from socket");
	  if (read_len == 0) {
            fprintf(stderr, "Empty message received, breaking. (fd=%d)\n", fd);
            break;
	  }
	}


        fprintf(stderr, "Incoming message from %s (fd=%d) (size=%d):\n", inet_ntoa(clientInfo->client_addr->sin_addr), fd, data->ndata);

        puts(data->data);

        
        handle(HANDLE_NEW_DATA, (void *) data);
    } while (1); 
    //send_msg_str(buf, fd);
    fprintf(stderr, "Terminating the receive loop for fd=%d\n", fd);
    handle(HANDLE_CONNECTION_BROKEN, (void *) clientInfo);
    if (close(fd))
        error("Error in closing the socket");
    return NULL;
}


pthread_t *issueCommunicationThread(int type, char *host, int port) {
  fprintf(stderr, "Comm thread host=%s, port=%d\n", host, port);
    pthread_t *thd = (pthread_t *) malloc(sizeof(pthread_t));
    int thread_ret;
    if (type == SERVER)
        thread_ret = pthread_create(thd, NULL, serve_thd, port);
    if (type == CLIENT) {
        struct ServerAddress *add = malloc(sizeof(struct ServerAddress));
        add->host = host;
        add->port = port;
        thread_ret = pthread_create(thd, NULL, client_thd, add);
    }
    if (thread_ret)
        puts("Error in thread creation.");
    return thd;
    
}

void error(char *msg) {
    perror(msg);
    exit(0);
}
