#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "server.h"
/* #include "server.c" */

#define BUFSIZE 1024
#define ECHO_PORT 20110
#define ECHO_BACKLOG 5

void* echo_server(void *conf)
{
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

                if (buffer[1] == '\n') {
                        printf("Closing client connection. Received empty Message");
                        fflush(stdout);
                        msg_len = 0;
                }
                
        }
        return (void *)buffer;
}

void echo() {
        if (lp_spawn(ECHO_PORT, &echo_server, ECHO_BACKLOG) != 0) {
                printf("Echo server running on port %d\n", ECHO_PORT);
        }
}

int main(void)
{
        echo();
        return 0;
}
