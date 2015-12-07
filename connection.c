#include <connection.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// --------------------------------------------------------------------

#define CONNECTION_BUFFER_SIZE 1024

// --------------------------------------------------------------------

connection_t* create_connection(int socket)
{
    connection_t* connection = (connection_t*)malloc(sizeof(connection_t));
    if (!connection)
    {
        fprintf(stderr, "Out of memory.\n");
        return NULL;
    }

    connection->socket = socket;
    connection->data_available = 0;
    connection->buffer = malloc(CONNECTION_BUFFER_SIZE);
    if (!connection->buffer)
    {
        free(connection);
        fprintf(stderr, "Out of memory.\n");
        return NULL;
    }

    connection->size = CONNECTION_BUFFER_SIZE;
    connection->pos = 0;

    return connection;
}

// --------------------------------------------------------------------

int connection_receive(connection_t* connection)
{
    connection->data_available = 0;

    int rv = recv(connection->socket,
                  connection->buffer + connection->pos,
                  connection->size,
                  0);

    if (rv >= 0)
    {
        connection->pos += rv;
    }

    return rv;
}

// --------------------------------------------------------------------

size_t connection_send(connection_t* connection, void* buffer, size_t count)
{
    return send(connection->socket, buffer, count, 0);
}

// --------------------------------------------------------------------

void destroy_connection(connection_t* connection)
{
    free(connection);
}

// --------------------------------------------------------------------

