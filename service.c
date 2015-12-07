#include <service.h>

#include <connection.h>
#include <server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------------

void reverse(char* buffer)
{
    size_t len = strlen(buffer);

/** Remove code, see Task 6.
    if (len > 512)
    {
        len = 512;
    }
*/
    for (size_t pos = 0; pos < len/2; ++pos)
    {
        char temp = buffer[pos];
        buffer[pos] = buffer[len - pos - 1];
        buffer[len - pos - 1] = temp;
    }
}

// --------------------------------------------------------------------

service_t* create_service(int port, unsigned max_connections)
{
    service_t* service = (service_t*)malloc(sizeof(service_t));
    if (!service)
    {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    memset(service, 0, sizeof(service_t));

    service->server = create_server(port, 16);
    if (!service->server)
    {
        fprintf(stderr, "Server creation failed, cannot proceed.\n");
        destroy_service(service);
        return NULL;
    }

    service->max_connections = max_connections;
    service->connections = malloc(sizeof(connection_t[max_connections]));
    if (!service->connections)
    {
        fprintf(stderr, "Out of memory, too many connections.\n");
        destroy_service(service);
        return NULL;
    }

    for (unsigned conn = 0; conn < max_connections; ++conn)
    {
        service->connections[conn] = NULL;
    }

    return service;
}

// --------------------------------------------------------------------

void destroy_service(service_t* service)
{
    if (service->connections)
    {
        for (unsigned conn = 0; conn < service->max_connections; ++conn)
        {
            if (service->connections[conn])
            {
                destroy_connection(service->connections[conn]);
                service->connections[conn] = NULL;
            }
        }
    }

    if (service->server)
    {
        destroy_server(service->server);
    }

    free(service);
}

// --------------------------------------------------------------------

int poll_sockets(service_t* service)
{
    unsigned count = service->max_connections;
    struct pollfd pfds[count + 1];
    nfds_t nfds = 0;

    pfds[nfds].fd = service->server->socket;
    pfds[nfds].events = POLLIN;
    ++nfds;

    for (unsigned conn = 0; conn < count; ++conn)
    {
        if (service->connections[conn])
        {
            pfds[nfds].fd = service->connections[conn]->socket;
            pfds[nfds].events = POLLIN;
            ++nfds;
        }
    }

    int rv;
    while ((rv = poll(pfds, nfds, -1)) == 0);

    nfds = 0;
    service->server->data_available = pfds[nfds].revents != 0;
    ++nfds;

    for (unsigned conn = 0; conn < count; ++conn)
    {
        if (service->connections[conn])
        {
            assert(service->connections[conn]->socket == pfds[nfds].fd);
            service->connections[conn]->data_available = pfds[nfds].revents != 0;
            ++nfds;
        }
    }

    return rv < 0 ? -1 : 0;
}

// --------------------------------------------------------------------

#define HANDLE_CONNECTION_ERROR -1
#define HANDLE_CONNECTION_CLOSED 0
#define HANDLE_CONNECTION_SUCCESS 1

// --------------------------------------------------------------------

int handle_connection(connection_t *conn)
{
    conn->data_available = 0;

    int rv = connection_receive(conn);
    if (rv <= 0)
    {
        return rv ? HANDLE_CONNECTION_ERROR : HANDLE_CONNECTION_CLOSED;
    }

    void *nl = NULL;
    void *p = NULL;

    nl = memchr(conn->buffer, '\n', conn->pos);
    if (nl)
    {
        size_t len = nl - conn->buffer;
        char* buffer = malloc(len + 2);
        if (!buffer)
        {
            fprintf(stderr, "Out of memory.\n");
            return HANDLE_CONNECTION_ERROR;
        }

        memcpy(buffer, conn->buffer, len);
        buffer[len] = buffer[len + 1] = 0;
        reverse(buffer);
        buffer[len] = '\n';
        connection_send(conn, buffer, len + 1);

        free(buffer);

//        memmove(conn->buffer, conn->buffer + len + 1, conn->size - conn->pos);
        conn->pos -= len + 1;
    }

    if (conn->pos == conn->size)
    {
        return HANDLE_CONNECTION_ERROR;
    }

    return HANDLE_CONNECTION_SUCCESS;
}

// --------------------------------------------------------------------

void handle_connections(service_t* service)
{
    for (unsigned id = 0; id < service->max_connections; ++id)
    {
        connection_t* conn = service->connections[id];

        if (conn && conn->data_available)
        {
            switch (handle_connection(conn))
            {
                case HANDLE_CONNECTION_SUCCESS:
                    continue;

                case HANDLE_CONNECTION_ERROR:
                    fprintf(stderr, "Failure in connection, destroying ...\n");
                    break;

                case HANDLE_CONNECTION_CLOSED:
                    fprintf(stderr, "Connection closed remotely ...\n");
                    break;
            }

            destroy_connection(conn);
            service->connections[id] = NULL;
        }
    }
}

// --------------------------------------------------------------------

void handle_server(service_t* service)
{
    if (!service->server->data_available)
    {
        return;
    }

    int free_connection;
    for (unsigned id = 0; id < service->max_connections; ++id)
    {
        if (!service->connections[id])
        {
            free_connection = id;
            break;
        }
    }

    if (free_connection != -1)
    {
        service->connections[free_connection] = server_accept(service->server);
    }
}

// --------------------------------------------------------------------

void service_run(service_t* service)
{
    while (1)
    {
        if (poll_sockets(service) < 0)
        {
            fprintf(stderr, "Failure during poll, cannot proceed.\n");
            break;
        }

        handle_connections(service);
        handle_server(service);
    }
}

// --------------------------------------------------------------------

