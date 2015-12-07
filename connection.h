#ifndef CONNECTION_H
#define CONNECTION_H

#include <string.h>

/**
 * Models a connection via socket including receive buffer.
 */
typedef struct
{
    // client connection socket
    int socket;

    // flag showing data availibility
    int data_available;

    // buffer containing data read
    void *buffer;

    // size of buffer
    unsigned size;

    // current read position
    unsigned pos;

} connection_t;

/**
 * Create a new connection object that manages a given socket.  The
 * resuling pointer must be freed using `destroy_connection'.
 */
connection_t* create_connection(int socket);

/**
 * Read data from connection and store it into the internal buffer.
 * The function blocks until data is available.  This function resets
 * the `data_available` flag.
 * Returns the number of bytes read on success, 0 if the connection
 * was closed remotely and -1 on error.
 */
int connection_receive(connection_t* connection);

/**
 * Send `count' bytes of data in `buffer' to the peer.  Returns number
 * of bytes send or -1 on error.
 */
size_t connection_send(connection_t* connection, void* buffer, size_t count);

/**
 * Close the connection and free all used resources.
 */
void destroy_connection(connection_t* connection);

#endif
