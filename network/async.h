#ifndef __ASYNC__
#define __ASYNC__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define ASYNC_IN EPOLLIN
#define ASYNC_ET EPOLLET

struct _async_s
{
    int epollfd;
    struct epoll_event * ev;
    int max_events;
};

typedef struct _async_s async_s;

void async_create(async_s * as, int max_events);

void async_close(async_s * as);

void async_add_fd(async_s * as, int sock, int events_type);

void async_add(async_s * as, int sock, void * data, int events_type);

void async_del(async_s * as, int sock);

int async_wait(async_s * as);

#endif
