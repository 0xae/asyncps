#include "networking.h"

namespace networking{

socket_t getSocket(int blocking){
    socket_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd <= 0){
        return EFAILED;
    }

    if(!blocking){
        int flags = fcntl(sockfd, F_GETFL, 0);
        if(flags < 0){
            close(sockfd);
            return EFAILED;                
        }

        int ret = fcntl(sockfd, F_SETFL, flags|O_NONBLOCK);
        if(ret != 0){
            close(sockfd);
            return EFAILED;        
        }
    
    }

    int flags = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(int));
    if(ret != 0){
        close(sockfd);
        return EFAILED;
    }
    
    return sockfd;
}

socket_t getNioSocket(){
    return getSocket(false);
}

int isSocketNonBlocking(socket_t sockfd){
    int flags = fcntl(sockfd, F_GETFL, 0);
    if(flags < 0){
        return EFAILED;                
    }
    return flags & O_NONBLOCK;
}

socket_t getNioSocketWith(struct sockaddr_in *addr, const char *host, int port){
    socket_t nioSocket = getSocket(false);
    if(nioSocket != EFAILED && addr != NULL){
        fillAddressWithHostPort(addr, host, port);    
    }
    return nioSocket;
}

int resolveName(const char *host, char *ipbuf){
    struct sockaddr_in sa;

    sa.sin_family = AF_INET;
    if(inet_aton(host, &sa.sin_addr) == 0) {
        struct hostent *he;

        he = gethostbyname(host);
        if(he == NULL){
            return EFAILED;
        }
        memcpy(&sa.sin_addr, he->h_addr, sizeof(struct in_addr));
    }

    strcpy(ipbuf, inet_ntoa(sa.sin_addr));
    return ESUCCESS;
}

int getSocketError(socket_t sock){
    int errorval;
    socklen_t size;
    int ret;
    
    size = sizeof(int);
    ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, &errorval, &size);
    if(ret != 0){
        return EFAILED;
    }

    return errorval;
}


int fillAddressWithHostPort(struct sockaddr_in *buffer,
                             const char *host, int port)
{
    bzero(buffer, sizeof(struct sockaddr_in));
    int ret = inet_pton(AF_INET, host, &buffer->sin_addr);
    if(ret != 1){
        return EFAILED;
    }

    buffer->sin_family = AF_INET;
    buffer->sin_port = htons(port);
    
    return ESUCCESS;
}


int fillAddressWithFd(struct sockaddr_in *buffer, socket_t fd){
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = getpeername(fd, castAsAddress(buffer), &len);
    if(ret != 0){
        return EFAILED;
    }

    bzero(buffer, sizeof(struct sockaddr_in));
    buffer->sin_family = AF_INET;
    
    return ESUCCESS;
}


int getHostFromAddress(const struct sockaddr_in *addr, char *host){
    const char *ret = inet_ntop(AF_INET,
                                &addr->sin_addr,
                                host,
                                sizeof(struct sockaddr_in));

    return (ret == NULL) ? EFAILED : ESUCCESS;
}

struct sockaddr_in* newAddressFromFd(socket_t sock){
    struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if(addr != NULL){
        if(fillAddressWithFd(addr, sock) == EFAILED){
            free(addr);
            return NULL;
        }
    }
    return addr;
}


struct sockaddr_in* newAddressFromHostPort(const char *host, int port){
    struct sockaddr_in *addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    if(addr != NULL){
        if(fillAddressWithHostPort(addr, host, port) == EFAILED){
            free(addr);
            return NULL;
        }
    }

    return addr;
}

int getHostPortFromFd(socket_t fd, char *host, int *port){
    struct sockaddr_in addr;
    if(fillAddressWithFd(&addr, fd) == EFAILED){
        return EFAILED;
    }

    if(getHostFromAddress(&addr, host) == EFAILED){
        return EFAILED;
    }

    *port = htons(addr.sin_port);
    
    return ESUCCESS;
}

int connectSocket(socket_t sock, const struct sockaddr *addr, socklen_t addrsize){
    int ret;
    do{
        ret = connect(sock, addr, addrsize);    
        if(ret < 0){
            if(errno == EINTR){
                continue;            
            }
            return EFAILED;
        }
    }while(false);

    return ESUCCESS;
}

int writeToSocket(socket_t sock, const char *buffer, int bufferSize){
    int written = 0;
    while(written < bufferSize){
        int w = write(sock, buffer+written, bufferSize-written);
        if(w == 0) break;
        if(w == -1) return EFAILED;
        written += w;
    }
    return written;
}

int readFromSocket(socket_t sock, char *buffer, int bufferSize){
    int readn = 0;
    while(readn < bufferSize){
        int r = read(sock, buffer+readn, bufferSize-readn);
        if(r == 0) break;
        if(r == -1) return EFAILED;
        readn += r;
    }
    return readn;
}

int bindSocket(socket_t sock, const struct sockaddr *addr, socklen_t addrsize){
    if(bind(sock, addr, addrsize) == 0){
        return ESUCCESS;
    }
    return EFAILED;
}

int listenToSocket(socket_t sock, int backlog){
    if(listen(sock, backlog) == 0){
        return ESUCCESS;
    }
    return EFAILED;
}

} // END namespace networking

