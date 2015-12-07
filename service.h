#ifndef SERVICE_H
#define SERVICE_H

#include <connection.h>
#include <server.h>

/**
 * Service object manages a server with a configured number of
 * maximum connections.
 */
typedef struct
{
    // pointer to server
    server_t* server;

    // maximum number of connections
    unsigned max_connections;

    // array of `max_connections' connection pointers
    connection_t** connections;

} service_t;

/**
 * Create a new service that runs a server listening on `port`
 * and serving up to `max_connections` simutaneously.
 */
service_t* create_service(int port, unsigned max_connections);

/**
 * Shut down server and connections and free memory used by the
 * service.
 */
void destroy_service(service_t* service);

/**
 * Run the service, accepting connection from the server and handling
 * input on the connections.
 */
void service_run(service_t* service);

#endif
