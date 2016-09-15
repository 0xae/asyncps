#ifndef ASYNCPS_CLIENT_H
#define ASYNCPS_CLIENT_H

#include <string>
#include <cstring>

namespace asyncps{

using std::string;

enum AsyncClientStatus{
    /* client connected to server */
    CONNECTED,

    /* still connecting */
    CONNECTING,

    /* closed client */
    DOWN
};

class AsyncClient{
    int fd;
    string buffer;
    AsyncClientStatus status;

public:
    /* connects client to (host, port) */
    AsyncClient(string host, int port, int blocking);

    /* a client created by
       an already connected socket 
    */
    AsyncClient(int connectedfd){
        fd = connectedfd;
        status = CONNECTED;
        buffer = "";
    }

    void storeMessage(string msg){ buffer = msg; }

    string getMessage() const { return buffer; }

    int getFd() const { return fd; }

    AsyncClientStatus getStatus() const {
        return status;
    }

    void setStatus(AsyncClientStatus s){
        status = s;
    }
};


} /* namespace asyncps */

#endif /* ASYNCPS_CLIENT_H */
