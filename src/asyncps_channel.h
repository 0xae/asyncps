#ifndef ASYNCPS_CHANNEL_H
#define ASYNCPS_CHANNEL_H
#include <list>
#include <string>

using std::list;
using std::string;

namespace asyncps{

class AsyncChannel{
    string id;
    list<int> subscribers;
    int publisherfd;
    string msg;

public:
    AsyncChannel(string channelId, int publisherFd){
        id = channelId;
        publisherfd = publisherFd;
        msg = "";
    }

    int getPublisher() const {
        return publisherfd;
    }

    string getId() const {
        return id;
    }

    void subscribe(int fd){
        subscribers.push_front(fd);
    }

    string getMessage() const {
        return msg;
    }

    bool publish(string msgToPublish){
        msg = msgToPublish;
        return true;
    }
};

} // namespace asyncps
#endif
