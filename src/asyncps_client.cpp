#ifndef ASYNCPS_CLIENT_H
#include "asyncps_client.h"
#endif

#include "networking.h"

namespace asyncps{

AsyncClient::AsyncClient(string host, int port, int blocking){
    fd = networking::getSocket(blocking);
    struct sockaddr_in address;
    const char *chost = host.c_str();
    networking::fillAddressWithHostPort(&address, chost, port);

    connect_again:
    int ret = networking::connectSocket(fd,
                                        castAsAddress(&address),
                                        sizeof(struct sockaddr_in));
    if(ret == EFAILED){
        if(errno == EINTR){ /* interrupted, try again */
            goto connect_again;
        }else if(errno == EINPROGRESS){
            status = CONNECTING;        
        }else{
            close(fd);
            status = DOWN;
        }
    }else{
        status = CONNECTED;
    }
}


} // namespace asyncps
