/***************************************************************************
 *   Copyright (C) 2010 by Shrutarshi Basu                                 *
 *   basus@lafayette.edu                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "server.h"

int start_server(int protocol, int port)
{
        pid_t pid;
        int socket;

        pid = fork();
        if (pid == 0){
                get_socket(port);
                listen(socket, BACKLOG);
                recv_conn(socket);
        }
}

int get_socket(int pt)
{
        /* Initialize a socket and bind it, returning the socket ID */
        
        struct addrinfo hints, *servinfo;
        int sock_fd;
        char port;

        /* Setup for the socket */
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags = AI_PASSIVE;
        sprintf(&port, "%d", pt);

        if ((status = getaddrinfo(NULL, &port, &hints, &servinfo)) != 0) {
                fprintf(stderr, "Address fetch error %s", gai_strerror(status));
                exit(1);
        }

        /* Create the socket and check for errors */
        sock_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

        if (sock_fd <= 0) {
                fprintf(stderr, "Fatal: Server thread error");
                exit(1);
        }

        /* Bind the socket */
        if (bind(sock_fd, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
                fprintf(stderr, "Fatal: Socket binding error");
                exit(1);
        }
        
        freeaddrinfo(servinfo);        // Done with this information
        return sock_fd;
}

void recv_conn(int socket)
{
        /* Accepts connections from clients and spins of a thread to communicate */
        /* with it */
        
        struct sockaddr_storage *client_addr;
        struct client *cl;
        int client_len, client_fd;
        
        while(1) {

                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_storage *) malloc(client_len);
                client_fd = accept(socket, (struct sockaddr *)client_addr, &client_len);

                if (client_fd == -1) {
                        fprintf(stderr, "Invalid client socket address");
                        continue;
                }

                cl = (struct client *) malloc(sizeof(struct client));
                cl->socket = client_fd;
                cl->client_addr = client_addr;

                pthread_t *receiver = (pthread_t *) malloc(sizeof(pthread_t));
                int thd = pthread_create(receiver, NULL, recv_msg, client);
        }
}

void *recv_msg(void *arg)
{
        /* Looping receiver that receives messages from the client and calls */
        /* the handler */
        
        struct client *client_info = (struct client *) arg;
        struct pollfd fds[1];
        int fd, read_len;
        unsigned int msg_len;

        fd = client_info->socket;
        fds->fd = fd;
        fds->events = POLLIN;
        
        do {
                //first read the number of bytes expected.

                read_len = recv(fd, &msg_len, 4, 0);
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

                int i;
                for (i = 0; i < transmission_len; i += read_len) {
                        read_len = recv(fd, data->data + i, transmission_len - i, 0);
                        if (read_len < 0)
                                error("ERROR reading from socket");
                        if (read_len == 0) {
                                fprintf(stderr, "Empty message received, breaking. (fd=%d)\n", fd);
                                break;
                        }
                }

                handle(HANDLE_NEW_DATA, (void *) data);
        } while (read_len);
        
        fprintf(stderr, "Terminating the receive loop for fd=%d\n", fd);
        handle(HANDLE_CONNECTION_BROKEN, (void *) clientInfo);
        
        if (close(fd))
                error("Error in closing the socket");
        return NULL;
}

void server_handler(int type, void *data)
{
        /* General purpose handler for each server thread. Differentiates
           events on the basis of event type and protocol being served.
         */
        
        struct IncomingData *idata;
        struct RemoteConnection client;
        
        idata = (struct IncomingData *) data;
        client = (struct RemoteConnection) idata->remoteConnection;

        switch(type) {
        case HANDLE_NEW_CONNECTION:
	        printf("New connection acquired.\n");
                printf("Client is port %d at address .\n",
                       client.client_addr->sin_port);
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
                printf("Connection with client broken.\n");
                printf("Client was port %d at address .\n",
                       client.client_addr->sin_port);
                pthread_exit(NULL);
                break;
        }
}
