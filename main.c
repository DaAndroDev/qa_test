#include <service.h>
#include <stdio.h>
#include <stdlib.h>

// --------------------------------------------------------------------

#define DEFAULT_PORT 8995
#define MAX_PARALLEL_CONNECTIONS 16

// --------------------------------------------------------------------

int main(int argc, char** argv)
{
    fprintf(stdout,
            "reversed - runs a server that reverses input ...\n\n"
            "syntax: reversed [port]\n"
            "\tport - change listening prot (default: %d)\n",
            DEFAULT_PORT);

    unsigned short port = DEFAULT_PORT;
    
    if (argc >= 2)
    {
        port = strtol(argv[1], 0, 10);
    }

    fprintf(stdout, "running on port %d\n", port);

    service_t* service = create_service(port, MAX_PARALLEL_CONNECTIONS);

    if (!service)
    {
        fprintf(stderr, "cannot create service, exiting ...\n");
        return 0;
    }

    service_run(service);

    destroy_service(service);

    return 0;
}
