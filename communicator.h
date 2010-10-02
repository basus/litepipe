/**
communicator.h
Berkin Ilbeyi

Common code functionality between the server and the client.
Manages sending/receiving through TCP/IP and the basic thread management.
The applications using this code need to define a handler functor with the signature
void (*)(int, void *) which this code will call on events of interest. The calls
are defined as preprocessor defines with the HANDLE_ prefix below.
*/
#ifndef __COMMUNICATOR_H__
#define __COMMUNICATOR_H__

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include "server.h"
#include "client.h"

//the thread mode
#define SERVER 0
#define CLIENT 1

//data will be RemoteConnection struct
#define HANDLE_NEW_CONNECTION 0
//data will be IncomingData struct
#define HANDLE_NEW_DATA 1
//data will be the RemoteConnection struct
#define HANDLE_CONNECTION_BROKEN 2
//data will be a char array with the error message
#define HANDLE_ERROR 3


#define COMMUNICATOR_BUF_SIZE 256

//structure represents a remote connection, that is a server if this is
//a client, or vice versa. 
struct client {
    int socket;                          //the fd of the socket
    struct sockaddr_storage *addr;       //the posix defined sockaddr_in struct
};

//wrapper structure for incoming data, to be passed on an incoming data to
//handle, with the handle type HANDLE_NEW_DATA
struct IncomingData {
    struct RemoteConnection remoteConnection;
  //the pointer for the data itself
    void *data;
  //size of the data in bytes
    unsigned int ndata;
};



void *recv_msg(void *);

//the functor to be triggered on an event
void (*handle)(int, void *);

//the function to set the handle functor to accept events from this.
void setHandler(void (*)(int status, void *data));

//initialazes the communication by issuing a thread to read from the socket
void communicate(struct RemoteConnection *);

//sends the data to the target with the given data and the data size
void sendData(struct RemoteConnection *, const void *data, int ndata);

//this is the main entry point of the communicator, the application code should
//call this after setting the handler. type indicates whether to be a client or a
//server and the port is the target port (or the local port if this is the server)
//host is ignored when type == SERVER
//this issues a thread for communication so this call is not blocking. 
//returns a reference to the thread created.
pthread_t *issueCommunicationThread(int type, char *host, int port);

//kills the given communication
void killCommunication(struct RemoteConnection *);

//the common catch-all error handler. passes the calls to the application with handle
void error(char *msg);

#endif
