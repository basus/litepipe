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

#include <pthread.h>
#include "server.h"

pthread_t *lp_spawn(int port, void* (*responder)(void*), int backlog)
{
        pid_t pid;
        int socket, server_success;
        struct listener_data *data;
        pthread_t *server;

        /* Create a socket for the port */
        socket = get_socket(port);

        /* Create a thread and assign the listener to it */
        data = (struct listener_data *) malloc(sizeof(*data));
        data->socket = socket;
        data->backlog = backlog;
        data->responder = responder;
        
        server = (pthread_t *) malloc(sizeof(pthread_t));
        server_success = pthread_create(server, NULL, (void*)listener, data);

        pthread_join((pthread_t)&server, NULL);
        if (server_success == 0) {
                return server;
        } else {
                fprintf(stderr, "Server thread error");
                return 0;
        }
        
}

int get_socket(int pt)
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

void listener(void *data)
{
        /* Accepts connections from clients and spins of a thread to communicate */
        /* with it */

        struct sockaddr_storage *client_addr;
        struct client *cl;
        struct listener_data *ldata;
        int client_len, *client_fd;

        ldata = (struct listener_data*)data;
        
        if (listen(ldata->socket, ldata->backlog) == -1) {
                fprintf(stderr, "Listening error");
        }
        
        while(1) {
                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_storage *) malloc(client_len);
                client_fd = (int *)malloc(sizeof(client_fd));
                *client_fd = accept(ldata->socket, (struct sockaddr *)client_addr, &client_len);

                if (*client_fd == -1) {
                        fprintf(stderr, "Invalid client socket address");
                        continue;
                }

                pthread_t *receiver = (pthread_t *) malloc(sizeof(pthread_t));
                int thd = pthread_create(receiver, NULL, ldata->responder, (void *)client_fd);
        }
        fprintf(stdout, "Finishing Listening");
}
