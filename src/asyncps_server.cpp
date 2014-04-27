#ifndef ASYNCPS_SERVER_H
#include "asyncps_server.h"
#endif

#include <iostream>
#include <cstring>
#include "logging.h"
#include "networking.h"

using std::map;
using std::list;
using logging::EVENT;
using logging::ERROR;
using logging::INFO;
using eventd::EventDaemon;
using eventd::ioOperation;
using eventd::Connection;

namespace asyncps {

static void writeOp(EventDaemon *event, socket_t sockfd, void *data);
static void readOp(EventDaemon *event, socket_t sockfd, void *data);
static void acceptOp(EventDaemon *event, socket_t serverfd, void *data);

AsyncServer::AsyncServer(string host, int port){
    fd = networking::getNioSocket();
    struct sockaddr_in address;
    const char *chost = host.c_str();

    networking::fillAddressWithHostPort(&address, chost, port);
    networking::bindSocket(fd,
                            castAsAddress(&address),
                            sizeof(struct sockaddr_in));
    networking::listenToSocket(fd, 500);
    event = new EventDaemon(30);
    event->registerFd(fd, EVENT_READ);
    event->setFdData(fd, this);
    event->setFdReadOp(fd, acceptOp);
    logging::log(INFO, "Server ready at %s:%d:%d\n", chost, port, fd);
}


AsyncServer::~AsyncServer(){
    delete event;
}


void AsyncServer::start(){
    event->beginEventLoop(-1, true);
}


void AsyncServer::registerClient(int fd){
    AsyncClient *client = new AsyncClient(fd);
    clients[fd] = client;
}


AsyncClient* AsyncServer::getClient(int fd){
    AsyncClient *client = clients[fd];
    return client;
}


void AsyncServer::deleteClient(int fd){
    AsyncClient *client = clients[fd];
    if(client != 0){
        delete client;
        clients[fd] = 0;
    }
}

void AsyncServer::parseRequest(int sockfd){
    AsyncClient *client = getClient(sockfd);
    string buffer = client->getMessage();
    const char *str = buffer.c_str();
    int pos = buffer.find(" ");

    if(pos == -1){
        client->storeMessage("Invalid command");
    }else{
        string command = buffer.substr(0, pos);

        if(command == "publish"){
            string buffer2 = buffer.substr(pos+1);
            int epos = buffer2.find(" ");
            string channelId = buffer2.substr(0, epos);
            string message = buffer2.substr(epos+1);

            if(channels[channelId] == 0){
                channels[channelId] = new AsyncChannel(channelId, sockfd);
                logging::log(INFO, "channel '%s' created\n",
                                     channelId.c_str());
            }

            AsyncChannel *channel = channels[channelId];
            logging::log(EVENT, "%d published '%s' to '%s'\n",
                                 sockfd, message.c_str(),
                                 channelId.c_str());

            if(sockfd == channel->getPublisher()){
                channel->publish(message);            
                client->storeMessage("ok");
            }else{
                client->storeMessage("not_yours");            
            }

        }else if(command == "subscribe"){
            string channelId = buffer.substr(pos+1);
            AsyncChannel *channel = channels[channelId];

            if(channel == 0){
                logging::log(ERROR, "request for invalid channel '%s'\n",
                                     channelId.c_str());
                client->storeMessage("nochannel");

            }else{
                client->storeMessage(channel->getMessage());
                logging::log(EVENT, "%d subscribed to channel '%s'\n",
                                    sockfd, channelId.c_str());
            }


        }else{
            logging::log(ERROR, "Invalid command '%s'\n", buffer.c_str());
            client->storeMessage("Invalid command");        
        }
    }

    event->updateFdEvents(sockfd, EVENT_READ|EVENT_WRITE);
    event->setFdWriteOp(sockfd, writeOp);
}



static void acceptOp(EventDaemon *event, socket_t serverfd, void *data){
    AsyncServer *server = (AsyncServer *)data;
    socket_t sockfd = accept(serverfd, 0 ,0);
    logging::log(EVENT, "%d connected\n", sockfd);

    server->registerClient(sockfd);

    // we wont install the writer op until
    // he sends us a message
    event->registerFd(sockfd, EVENT_READ);
    event->setFdReadOp(sockfd, readOp);

    // data is an AsyncServer instance
    event->setFdData(sockfd, data);
}


static void readOp(EventDaemon *event, socket_t sockfd, void *data){
    AsyncServer *server = (AsyncServer*)data;
    char msg[500];
    memset(msg, 0, sizeof(msg));
    int r = read(sockfd, msg, sizeof(msg));

    // closed connection
    if(r == 0){
        logging::log(EVENT, "%d disconnected\n", sockfd);
        event->unregisterFd(sockfd);
        server->deleteClient(sockfd);
        close(sockfd);
        return;
    }

    AsyncClient *client = server->getClient(sockfd);
    client->storeMessage(msg);
    server->parseRequest(sockfd);
}


static void writeOp(EventDaemon *event, socket_t sockfd, void *data){
    AsyncServer *server = (AsyncServer *)data;
    AsyncClient *client = server->getClient(sockfd);
    const char *msg = client->getMessage().c_str();

    logging::log(EVENT, "%d sent '%s'\n", sockfd, msg);
    logging::log(INFO, "Server sent '%s' to %d\n", msg, sockfd);

    int len = strlen(msg);
    write(sockfd, msg, len);
    client->storeMessage("");

    // we wont send him a message
    // until he sends us a message
    event->updateFdEvents(sockfd, EVENT_READ);
    event->setFdWriteOp(sockfd, 0);
}



} // namespace asyncps



