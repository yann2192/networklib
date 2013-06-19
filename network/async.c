#include "async.h"

void async_create(async_s * as, int max_events)
{
    /* Création de l'instance epoll */
    as->epollfd = epoll_create(max_events);
    if(as->epollfd < 0){
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }
    as->max_events = max_events;
    as->ev = malloc(sizeof(struct epoll_event)*max_events);
}

void async_close(async_s * as)
{
    close(as->epollfd);
    free(as->ev);
    as->max_events = 0;
}

void async_add_fd(async_s * as, int sock, int events_type)
{
    as->ev->events = events_type;
    as->ev->data.fd = sock;
    /* Ajout du fd du server au epoll */
    if(epoll_ctl(as->epollfd, EPOLL_CTL_ADD, sock, as->ev) < 0){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void async_add(async_s * as, int sock, void * data, int events_type)
{
    as->ev->events = events_type;
    as->ev->data.ptr = data;
    /* Ajout du fd du server au epoll */
    if(epoll_ctl(as->epollfd, EPOLL_CTL_ADD, sock, as->ev) < 0){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void async_del(async_s * as, int sock)
{
    if(epoll_ctl(as->epollfd, EPOLL_CTL_DEL, sock, as->ev) < 0){
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

int async_wait(async_s * as)
{
    int tmp;
    do {
        tmp = epoll_wait(as->epollfd, as->ev, as->max_events, -1);
        if(tmp < 0 && errno != EINTR){
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
    } while(tmp < 0 && errno == EINTR);
    return tmp;
}
