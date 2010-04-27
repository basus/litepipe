/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "server.h"
#include <pthread.h>

int start_server()
{
        signal(SIGINT, sigint_handler);
        setHandler(&server_handler);
        pthread_t *timeThr = issueCommunicationThread(SERVER, NULL, TIME);
        pthread_t *infoThr = issueCommunicationThread(SERVER, NULL, INFO);
        pthread_t *httpThr = issueCommunicationThread(SERVER, NULL, HTTP);
        //while(1);
	pthread_join(*timeThr, NULL);
	pthread_join(*infoThr, NULL);
	pthread_join(*httpThr, NULL);
}

void sigint_handler(int sig)
{
        printf("Now Exiting Litepipe Server.\n");
        printf("Outstanding requests may go unanswered.\n");
        exit(0);
}

void *serve_thd(int pt)
{
        struct addrinfo hints, *servinfo;
        struct sockaddr_in *client_addr;
        int sock_fd, client_fd, status, client_len;

        char port;
        sprintf(&port, "%d", pt);
        
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags = AI_PASSIVE;

        if ((status = getaddrinfo(NULL, &port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "Addres fetch error %s", gai_strerror(status));
                exit(1);
        }

        sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        if (sock_fd <= 0) {
                fprintf(stderr, "Server thread error");
        }

        if (bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
                fprintf(stderr, "Socket binding error");
                exit(1);
        }
        
        freeaddrinfo(servinfo);        // Done with this information

        listen(sock_fd, 5);
        printf("Litepipe server now running on port %d\n", pt);
        fflush(stdout);

        while(1) {
                fprintf(stderr, "Waiting for requests\n");
                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
                client_fd = accept(sock_fd, (struct sockaddr *)client_addr, &client_len);

                if (client_fd == -1) {
                        fprintf(stderr, "Invalid client socket address");
                        continue;
                }

                struct RemoteConnection *client = (struct RemoteConnection *) malloc(sizeof(struct RemoteConnection));
                client->comm_sock_fd = client_fd;
                client->client_addr = client_addr;
                client->protocol = pt;

                communicate(client);
        }

        close(sock_fd);
}

void server_handler(int type, void *data)
{
        struct IncomingData *idata;
        struct RemoteConnection client;
        
        idata = (struct IncomingData *) data;
        client = (struct RemoteConnection) idata->remoteConnection;
        char *ip;
        
        switch(type) {
        case HANDLE_NEW_CONNECTION:
	        ip = (char*)inet_ntoa(client.client_addr->sin_addr);
                printf("New connection acquired.\n");
                printf("Client is port %d at address %s.\n",
                       client.client_addr->sin_port,
                       ip);
                break;
        case HANDLE_NEW_DATA:
                switch (client.protocol) {
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
                ip = (char*)inet_ntoa(client.client_addr->sin_addr);
                printf("Connection with client broken.\n");
                printf("Client was port %d at address %s.\n",
                       client.client_addr->sin_port,
                       ip);
                pthread_exit(NULL);
                break;
        }
}
