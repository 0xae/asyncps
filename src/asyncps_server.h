#ifndef ASYNCPS_SERVER_H
#define ASYNCPS_SERVER_H

#include <map>
#include <list>
#include "eventd.h"
#include "networking.h"
#include "asyncps_channel.h"
#include "asyncps_client.h"

using std::map;
using std::string;
using networking::socket_t;
using eventd::EventDaemon;
using asyncps::AsyncChannel;
using asyncps::AsyncClient;

namespace asyncps{

class AsyncServer {
    int fd;
    map<string, AsyncChannel*> channels;
    map<int, AsyncClient*> clients;
    EventDaemon *event;

public:
    AsyncServer(string host, int port);
    ~AsyncServer();

    AsyncClient* getClient(int fd);
    int getFd() const { return fd; };
    void registerClient(int fd);
    void deleteClient(int fd);
    void start();
    void parseRequest(int sockfd);
};

} /* namespace asyncps */

#endif /* ASYNCPS_SERVER_H */


