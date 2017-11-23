
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "msg.h"
#include "db-worker.h"
#include "tm-types.h"

// =============================================================================
// Declaration of all server variables
// =============================================================================

int idCounter = MIN_UID;
int serverSockFD;               // Socket descriptor for communication
fd_set master;                  // Set of socket file descriptors
fd_set read_fds;                // Set of socket file descriptor to read
int fdmax;                      // Maximal number of sockets
int result;                     // Used to analyze return code of functions
struct sockaddr_in clnt_addr;
struct sockaddr_in serv_addr;


// =============================================================================
// Set of main functions
// =============================================================================

// Name        : server_form_refresh_answer()
// Description : function form string that contain information about
//               client connected to server at that moment.
char* server_form_refresh_answer(char *filePath, int *counter);

// Name	 	   : server_receive_packet()
// Description : Function used to take data from client.
// Functional  : Function works full time.
void* server_receive_packet(int sFD, fd_set *master, int serverSockFD, int fdmax);

// Name        : server_connection_accept()
// Description : Function creates new connection.
void server_connection_accept(fd_set *master, int *fdmax, int serverSockFD, struct sockaddr_in *client_addr);


// Name        : server_send_to()
// Description : Function send packet structure.
void server_send_to(int dest_sFD, int serverSockFD, struct packet *msg, fd_set *master);

#endif /* SERVER_H */
