#include <server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --------------------------------------------------------------------

server_t* create_server(int port, int backlog)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        perror("socket");
        return NULL;
    }

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    {
        fprintf(stderr, "Warning: could not set SO_REUSEADDR\n");
    }

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = port;
    sin.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        close(s);
        return NULL;
    }

    if (listen(s, backlog) < 0)
    {
        perror("listen");
        close(s);
        return NULL;
    }

    server_t* server = (server_t*)malloc(sizeof(server_t));
    if (!server)
    {
        fprintf(stderr, "Out of memory.\n");
        close(s);
        return NULL;
    }

    server->socket = s;
    server->data_available = 0;

    return server;
}

// --------------------------------------------------------------------

connection_t* server_accept(server_t* server)
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(struct sockaddr_in);

    int sock = accept(server->socket, (struct sockaddr*)&sin, &len);
    if (sock < 0)
    {
        perror("accept");
        return NULL;
    }

    fprintf(stdout,
            "accepted new conncetion from %s:%i\n",
            inet_ntoa(sin.sin_addr),
            sin.sin_port);

    connection_t* connection = create_connection(sock);
    if (!connection)
    {
        fprintf(stderr, "could not create connection object\n");
        close(sock);
    }

    return connection;
}

// --------------------------------------------------------------------

void destroy_server(server_t* server)
{
    if (close(server->socket) < 0)
    {
        perror("close");
    }

    free(server);
}

// --------------------------------------------------------------------

