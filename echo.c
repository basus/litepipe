#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "litepipe.h"

#define BUFSIZE 1024
#define ECHO_PORT 20110
#define ECHO_BACKLOG 5

void* echo_server(void *conf)
{
        /* Echo server to respond to a single client */
        int *socket, msg_len;
        char *buffer;

        buffer = (char *)malloc(BUFSIZE);
        socket = (int *)conf;
        msg_len = 1;
        
        while(msg_len > 0) {
                msg_len = recv(*socket, (char *)buffer, BUFSIZE-1, 0);
                
                if (msg_len < 0) {
                        printf(strerror(errno));
                        fflush(stdout);
                        return NULL;
                }

                send(*socket, (char *)buffer, msg_len, 0);

                /* Close the connection on an empty message */
                if (buffer[1] == '\n') {
                        printf("Closing client connection. Received empty Message");
                        fflush(stdout);
                        msg_len = 0;
                }
                
        }
        return (void *)buffer;
}

int main()
{
        pthread_t *server;
        server = lp_spawn(ECHO_PORT, ECHO_BACKLOG, &echo_server);
        if (server != -1) {
                printf("Echo server running successfully");
                fflush(stdout);
        }
        pthread_join(*server);
        return 0;
}
