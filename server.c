/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "communicator.h"
#include "server.h"
int serve(int);

//void recv_msg(int);



void *client_conn_thread(void *);

/*
int main(int argc, char *argv[]) {
    if (argc < 2) {
	fprintf(stderr,"ERROR, no port provided\n");
	exit(1);
    }
    int portno = atoi(argv[1]);
    return serve(portno);
}
*/

void *serve_thd(void *arg) {
    return (void *) serve((int) arg);
}

int serve(int port_no) {
    int sock_fd, comm_sock_fd, clilen;
    char buffer[256];
    struct sockaddr_in server_addr, *client_addr;
    int n;

    //open socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        error("ERROR opening socket");

    //clear out the server address
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_no);

    if (bind(sock_fd, (struct sockaddr *) &server_addr,
             sizeof(server_addr)) < 0)
        error("ERROR on binding");
    listen(sock_fd, 5);
    for (;;) {


        clilen = sizeof(client_addr);
        client_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
        comm_sock_fd = accept(sock_fd,
                              (struct sockaddr *) client_addr,
                              &clilen);
        if (comm_sock_fd < 0)
            error("ERROR on accept");
        bzero(buffer, 256);

        struct RemoteConnection *clientInfo = (struct ClientInfo *) malloc(sizeof(struct RemoteConnection));
        clientInfo->comm_sock_fd = comm_sock_fd;
        clientInfo->client_addr = client_addr;

        pthread_t *thd = (pthread_t *) malloc(sizeof(pthread_t));
        int thread_ret = pthread_create(thd, NULL, client_conn_thread, clientInfo);
        if (thread_ret)
            puts("Error in thread creation/");
        //communicate(comm_sock_fd, &client_addr);
    }
    //recv_msg(comm_sock_fd);

    //printf("Here is the message: %s\n",buffer);
    //n = write(comm_sock_fd,"I got your message",18);
    //if (n < 0)
    //error("ERROR writing to socket");
    return 0;

}
/*
void issue_serve_thread(int portNo) {
    pthread_t *thd = (pthread_t *) malloc(sizeof(pthread_t));
    int thread_ret = pthread_create(thd, NULL, serve_thd, portNo);
    if (thread_ret)
        puts("Error in thread creation.");
}
*/

void *client_conn_thread(void *_clientInfo) {
    struct RemoteConnection *clientInfo = (struct RemoteConnection *) _clientInfo;
    fprintf(stderr, "client thread fd=%d\n", clientInfo->comm_sock_fd);
    communicate(clientInfo);
    fprintf(stderr, "client thread exit\n");
    return NULL;

}
