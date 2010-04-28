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

int start_server()
{
        /* Sets up exit signal handler, creates threads for time, info and HTML
           and then waits for threads to finish
         */
        
        signal(SIGINT, sigint_handler);
        setHandler(&server_handler);
        
        pthread_t *timeThr = issueCommunicationThread(SERVER, NULL, TIME);
        pthread_t *infoThr = issueCommunicationThread(SERVER, NULL, INFO);
        pthread_t *httpThr = issueCommunicationThread(SERVER, NULL, HTTP);

	pthread_join(*timeThr, NULL);
	pthread_join(*infoThr, NULL);
	pthread_join(*httpThr, NULL);
}

void sigint_handler(int sig)
{
        /* Just exits server */
        
        printf("\nNow Exiting Litepipe Server.\n");
        printf("Outstanding requests may go unanswered.\n");
        printf("Clients are responsible for dealing with dropped connections.\n");
        exit(0);
}

void *serve_thd(int pt)
{
        /* Creates sockets and connections for the server and then accepts
           clients, handing off to the common library to handle communication
           setup
        */
        
        struct addrinfo hints, *servinfo;
        struct sockaddr_in *client_addr;
        int sock_fd, client_fd, status, client_len;
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

        listen(sock_fd, 5);
        printf("Litepipe server now listening on port %d\n", pt);
        fflush(stdout);

        while(1) {

                client_len = sizeof(client_addr);
                client_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
                client_fd = accept(sock_fd, (struct sockaddr *)client_addr, &client_len);

                if (client_fd == -1) {
                        fprintf(stderr, "Non-fatal: Invalid client socket address; listening for connections");
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
