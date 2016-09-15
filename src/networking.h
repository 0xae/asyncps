#ifndef NETWORKING_H
#define NETWORKING_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <sys/time.h> /* timeval{} for select() */
#include <time.h>  /* timespec{} for pselect() */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /* inet(3) functions */
#include <errno.h>
#include <fcntl.h>  /* for nonblocking */
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <sys/uio.h>  /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>  /* for Unix domain sockets */
#include <assert.h>
#include <sys/select.h> /* for convenience */
#include <poll.h>  /* for convenience */
#include <strings.h>  /* for convenience */
#include <sys/ioctl.h>
#include <pthread.h>

#ifdef true
#undef true
#endif

#ifdef false
#undef false
#endif

#ifdef bool
#undef bool
#endif

#define bool char
#define true  1
#define false 0

#ifdef bzero
#undef bzero
#endif

#define bzero(pointer, size) memset(pointer, 0, size)

namespace networking{

#define NOTUSED(arg) ((void)(arg))

#define ENOT_IN_RANGE  -2
#define ENOT_ACTIVE    -3
#define EFAILED        -4
#define ESUCCESS        1
#define NOFD           -1

#define fill_addr_with_hostport  fillAddressWithHostPort
#define fill_addr_with_fd        fillAddressWithFd
#define get_host_from_addr       getHostFromAddress

#define new_addr_from_fd         newAddressFromFd
#define new_addr_from_hostport   newAddressFromHostPort
#define get_port_from_addr(addr) getPortFromAddress

#define castAsAddress(addr)     (struct sockaddr *)(addr)
#define castAsAddressIn4(addr)  (struct sockaddr_in *)(addr)
#define castAsAddressIn6(addr)  (struct sockaddr_in6 *)(addr)
#define getPortFromAddress(addr) (addr)->sin_port
#define nio_socket getNioSocket

typedef int socket_t;

int getSocketError(socket_t sock);
int fillAddressWithHostPort(struct sockaddr_in *buffer, const char *host, int port);
int fillAddressWithFd(struct sockaddr_in *buffer, socket_t fd);
int getHostFromAddress(const struct sockaddr_in *addr, char *host);
int getHostPortFromFd(socket_t fd, char *host, int *port);
struct sockaddr_in* newAddressFromFd(socket_t sock);
struct sockaddr_in* newAddressFromHostPort(const char *host, int port);

socket_t getSocket(int blocking);
socket_t getNioSocket();
int isSocketNonBlocking(socket_t sock);
socket_t getNioSocketWith(struct sockaddr_in *addr, const char *host, int port);

int connectSocket(socket_t sock, const struct sockaddr *addr, socklen_t addrsize);
int writeToSocket(socket_t sock, const char *buffer, int bufferSize);
int readFromSocket(socket_t sock, char *buffer, int bufferSize);
int bindSocket(socket_t sock, const struct sockaddr *addr, socklen_t addrsize);
int listenToSocket(socket_t sock, int backlog);
int resolveName(const char *host, char *ipbuf);

} // END namespace networking

#endif

