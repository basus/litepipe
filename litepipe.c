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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#include "litepipe.h"

pthread_t *lp_spawn(int port, void* (*handler)(void*), int backlog)
{
        /* Creates a socket on the given port and then spawns a listener for
           it using the given function.  */
        
        pid_t pid;
        int socket, server_success;
        struct responder *rsp;
        pthread_t *server;

        /* Create a socket for the port */
        socket = lp_get_socket(port);

        /* Create a structure to hold data required by the listener */
        rsp = (struct responder *) malloc(sizeof(*rsp));
        rsp->socket = socket;
        rsp->backlog = backlog;
        rsp->handler = handler;
        
        server = (pthread_t *) malloc(sizeof(pthread_t));
        server_success = pthread_create(server, NULL, (void*)lp_listener, rsp);

        if (server_success == 0) {
                return server;
        } else {
                fprintf(stderr, "Server thread error");
                return 0;
        }
        
}

int lp_get_socket(int pt)
{
        /* Initialize a socket and bind it, returning the socket ID */
        
        struct addrinfo hints, *servinfo;
        int sock_fd, status;
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

void lp_listener(void *data)
{
        /* Accepts connections from clients and spins of a thread to communicate */
        /* with it */

        struct sockaddr_storage *client_addr;
        struct responder *rsp;
        int client_len, *client_fd;

        rsp = (struct responder*)data;
        
        if (listen(rsp->socket, rsp->backlog) == -1) {
                fprintf(stderr, "Listening error");
        }
        
        while(1) {
                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_storage *) malloc(client_len);
                client_fd = (int *)malloc(sizeof(client_fd));
                *client_fd = accept(rsp->socket, (struct sockaddr *)client_addr, &client_len);

                if (*client_fd == -1) {
                        fprintf(stderr, "Invalid client socket address");
                        continue;
                }

                /* Spawn a thread to communicate with a particular client */
                pthread_t *receiver = (pthread_t *) malloc(sizeof(pthread_t));
                int thd = pthread_create(receiver, NULL, rsp->handler, (void *)client_fd);
        }
}
