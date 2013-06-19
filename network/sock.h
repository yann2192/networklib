#ifndef __SOCK__
#define __SOCK__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>

typedef struct addrinfo * Saddr;

int sock_connect(char *, char *);
int sock_bind(char *, char *);
int accept_one(int);
int sendall(int, unsigned char *, int);
int recv_data(int, unsigned char *, int);
int recvall(int, unsigned char *, int);
void set_async(int);
void close_one(int);

#endif
