#ifndef EVENTD_H
#define EVENTD_H

#include <poll.h>
#include <errno.h>
#define NOTUSED(arg) ((void)(arg))

#define ENOT_IN_RANGE  -2
#define ENOT_ACTIVE    -3
#define EFAILED        -4
#define ESUCCESS        1
#define NOFD           -1

#define EVENT_READ  POLLRDNORM
#define EVENT_WRITE POLLWRNORM
#define EVENT_ERROR POLLERR
#define NOFD -1
#define POLL_INFINIT -1

namespace eventd{

class EventDaemon;
struct Connection;

typedef void (*ioOperation)(EventDaemon *event, int sockfd, void *data);

struct Connection{
    int fd;
    int events;
    void *data;

    ioOperation readOp;
    ioOperation writeOp;
    ioOperation errorOp;
};


class EventDaemon{
    struct Connection *array;
    int active;
    int size;
    
    // implementation specifique variables
    void *implArray;
    int implDataSize;

public:
    EventDaemon(int eventSize);
    ~EventDaemon();

    struct Connection* getConnectionFromFd(int fd);
    int registerFd(int fd, int events);
    int unregisterFd(int fd);

    int isFdReadable(int fd);
    int isFdWriteable(int fd);
    int fdHasError(int fd);
    int updateFdEvents(int fd, int events);

    int setFdReadOp(int fd, ioOperation readFd);
    int setFdWriteOp(int fd, ioOperation writeFd);
    int setFdErrorOp(int fd, ioOperation catchFd);

    int setFdData(int fd, void *data);
    void* getFdData(int fd);

    int getTotalConnections();
    int beginEventLoop(int timeout, int forever);
};


} // END eventd namespace

#endif

