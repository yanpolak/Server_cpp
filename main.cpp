#include "server.hpp"

int main(int argc, char **argv) 
{
    if (argc != 2)
    {
        write(2, "Usage: ./server <port>\n", 23);
        return 1;
    }

    int port = atoi(argv[1]);

    Server server;
    server.run(port);

    return 0;
}
