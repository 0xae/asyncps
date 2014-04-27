#include <iostream>
#include "eventd.h"
#include "logging.h"

using logging::WARNING;
using logging::ERROR;
using logging::EVENT;

namespace eventd{

#define isFdActive(fd1) ((fd1) >= 0 &&                 \
                              (fd1) < (this)->size &&        \
                              (this)->array[(fd1)].fd != -1) \

#define isFdInRange(fd) (fd >= 0 &&                \
                                  fd < (this)->size)

#define getPollFd(fd) &((struct pollfd *)(this)->implArray)[fd]


EventDaemon::EventDaemon(int eventSize){
    array = new Connection[eventSize];
    implArray = new pollfd[eventSize];
    implDataSize = sizeof(struct pollfd) * eventSize;

    struct pollfd *pollArray = (struct pollfd *)implArray;

    size = eventSize;
    active = 0;

    int i;
    for(i=0; i<eventSize; i++){
        array[i].fd = NOFD;
        array[i].events = 0;
        array[i].readOp = 0;
        array[i].writeOp = 0;
        array[i].errorOp = 0;
        array[i].data = 0;

        pollArray[i].fd = NOFD;
        pollArray[i].events = 0;
    }

}


EventDaemon::~EventDaemon(){
    delete[] (struct pollfd *)implArray;
    delete[] array;
}


struct Connection* EventDaemon::getConnectionFromFd(int fd){
    return &array[fd];
}


int EventDaemon::registerFd(int fd, int events){
    if(!isFdInRange(fd))
        return ENOT_IN_RANGE;

    struct Connection *connection = &array[fd];
    struct pollfd *pollConnection = getPollFd(fd);

    if(connection->fd == NOFD){
        pollConnection->fd = fd;
        pollConnection->events = events;

        connection->fd = fd;
        active += 1;

        return ESUCCESS;

    }else{
        #ifdef DEBUG
        logging::log(WARNING, "attempt to register"
                              " fd %d twice(ignoring)\n", fd);
        #endif
        return EFAILED;   
    }
}



int EventDaemon::unregisterFd(int fd){
    if(isFdActive(fd)){
        struct Connection *connection = &array[fd];    
        struct pollfd *pollConnection = getPollFd(fd);
        
        pollConnection->fd = NOFD;
        pollConnection->events = 0;
        
        connection->fd = NOFD;
        connection->events = 0;
        connection->readOp = 0;
        connection->writeOp = 0;
        connection->errorOp = 0;

        return ESUCCESS;
    }else{
        #ifdef DEBUG
        logging::log(WARNING,"attempt to unregister"
                             " not registered fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }
}


int EventDaemon::isFdReadable(int fd){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING, "attempt to check"
                             " unactive fd %d for reading(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }


    struct pollfd *pollConnection = getPollFd(fd);
    return pollConnection->revents & EVENT_READ;
}

int EventDaemon::fdHasError(int fd){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to check"
                             " unactive fd %d for errors(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct pollfd *pollConnection = getPollFd(fd);
    return pollConnection->revents & EVENT_ERROR;
}

int EventDaemon::isFdWriteable(int fd){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to check"
                             " unactive fd %d for writing(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct pollfd *pollConnection = getPollFd(fd);
    return pollConnection->revents & EVENT_WRITE;
}

void * EventDaemon::getFdData(int fd){
    if(!isFdActive(fd)){
        return NULL;
    }

    return array[fd].data;
}

int EventDaemon::updateFdEvents(int fd, int events){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to change events "
                             " of unactive fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct Connection *connection = &array[fd];
    struct pollfd *pollConnection = getPollFd(fd);
    connection->events = events;
    pollConnection->events = events;
    return ESUCCESS;
}

int EventDaemon::setFdData(int fd, void *data){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to change data "
                             "of unactive fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct Connection *connection = &array[fd];    
    connection->data = data;
    return ESUCCESS;
    
}


int EventDaemon::setFdReadOp(int fd, ioOperation readOp){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to change read I/O "
                             "of unactive fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct Connection *connection = &array[fd];    
    connection->readOp = readOp;
    return ESUCCESS;
}


int EventDaemon::setFdWriteOp(int fd, ioOperation writeOp){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to change write I/O "
                             "of unactive fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct Connection *connection = &array[fd];    
    connection->writeOp = writeOp;
    return ESUCCESS;
}

int EventDaemon::setFdErrorOp(int fd, ioOperation errorOp){
    if(!isFdActive(fd)){
        #ifdef DEBUG
        logging::log(WARNING,"attempt to change catch I/O "
                             "of unactive fd %d(ignoring)\n", fd);
        #endif
        return ENOT_ACTIVE;
    }

    struct Connection *connection = getConnectionFromFd(fd);
    connection->errorOp = errorOp;
    return ESUCCESS;
}

int EventDaemon::getTotalConnections(){
    return active;
}

int EventDaemon::beginEventLoop(int timeout, int forever){
    struct pollfd *connections = (struct pollfd *)implArray;
    if(active == 0){
        #ifdef DEBUG
            logging::log(WARNING,"terminating event loop! "
                                 "cannot pool with 0 fds\n");
            perror("beginEventLoop()");
        #endif
        return ENOT_ACTIVE;
    }

    int n, i;
    struct pollfd *pfd = 0;
    struct Connection *conn = 0;
    do{
        n = poll(connections, size, timeout);
        if(n < 0){
            if(errno == EINTR){
                continue; 
            }
            #ifdef DEBUG
                logging::log(ERROR, "Error on event loop, "
                                     "terminating pooling\n");
                perror("poll()");
            #endif
            break;
        }

        for(i=0; i<size; i++){
            pfd = &connections[i];
            if(isFdActive(pfd->fd) == false){
                continue;
            }
            conn = &array[pfd->fd];

            if(isFdReadable(conn->fd)){
                if(conn->readOp != 0){                    
                    conn->readOp(this, conn->fd, conn->data);
                }
            }
            
            if(isFdWriteable(conn->fd)){
                if(conn->writeOp != 0){
                    conn->writeOp(this, conn->fd, conn->data);
                }
            }
            
            if(fdHasError(conn->fd)){
                if(conn->errorOp != 0){
                    conn->errorOp(this, conn->fd, conn->data);
                }
            }
        }
        
    }while(forever);

    return n;
}

} // END namespace eventd
