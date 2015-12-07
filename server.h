#ifndef SERVER_H
#define SERVER_H

#include <connection.h>

/**
 * This struct models a server socket.
 */
typedef struct
{
    // socket that is used to accept connections.
    int socket;

    // set to non-zero if data is available
    int data_available;

} server_t;

/**
 * Create a server that accepts connections on a given port.
 */
server_t* create_server(int port, int backlog);

/**
 * Block until a connection is available, return the connection object.
 */
connection_t* server_accept(server_t* server);

/**
 * Destroy a server object.
 */
void destroy_server(server_t* server);

#endif
