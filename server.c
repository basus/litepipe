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
#include "protocol.h"
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

int main()
{
        setHandler(server_handler);
        issueCommunicationThread(SERVER, NULL, TIME);
        issueCommunicationThread(SERVER, NULL, INFO);
        issueCommunicationThread(SERVER, NULL, HTTP);
}

void *serve_thd(void *arg) {
    return (void *) serve((int) arg);
}

void serve(int port)
{
        struct addrinfo hints, *servinfo;
        struct sockaddr_in *client_addr;
        int sock_fd, client_fd, status, client_len;
        
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "Addres fetch error");
                exit(1);
        }

        sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        if (bind(sock_fd, servinfo->ai_addr, res->ai_addrlen) != 0) {
                fprintf(stderr, "Socket binding error");
                exit(1);
        }

        listen(sock_fd, 5);

        while(1) {
                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
                client_fd = accept();

                if (client_fd != 0) {
                        fprintf(stderr, "Invalid client socket address");
                        continue;
                }

                struct RemoteConnection *client = (struct ClientInfo *) malloc(sizeof(struct RemoteConnection));
                client->comm_sock_fd = client_fd;
                client->client_addr = client_addr;
                client->protocol = port;

                communicate(client);
        }
}

void server_handler(int type, void *data)
{
        switch(type) {
        case HANDLE_NEW_CONNECTION:
                break;
        case HANDLE_NEW_DATA:
                struct incomingData *idata = (incomingData *) data;
                struct RemoteConnection *client = (RemoteConnection *) idata->remoteConnection;
                
                switch (client->protocol) {
                case TIME:
                        serve_time(data);
                        break;
                case INFO:
                        serve_info(data);
                        break;
                case HTTP:
                        serve_http(data);
                        break;
                }
                break;
        case HANDLE_CONNECTION_BROKEN:
        }
}
