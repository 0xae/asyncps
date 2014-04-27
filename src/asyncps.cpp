#include "asyncps_server.h"
#include <stdlib.h>
#include <assert.h>

using asyncps::AsyncServer;

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    string host = argv[1];
    int port = atoi(argv[2]);
    assert(port > 0);

    AsyncServer server(host, port);
    server.start();
    return 0;
}


