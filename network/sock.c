#include "sock.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


static Saddr getaddr(char * host, char * port)
{
    Saddr addr = 0;
    if(getaddrinfo(host, port, 0, &addr)){
        perror("getaddrinfo");
        return 0;
    }
    return addr;
}

static void freeaddr(Saddr addr)
{
    freeaddrinfo((struct addrinfo *)addr);
}

int sock_connect(char * host, char * port)
{
    int sock;
    Saddr addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr = getaddr(host, port);
    if(!addr) return -1;
    if(connect(sock, (struct sockaddr *)addr->ai_addr, addr->ai_addrlen)){
        perror("connect");
        return -1;
    }
    freeaddr(addr);
    return sock;
}

int sock_bind(char * host, char * port)
{
    int sock, on;
    Saddr serv_addr = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        handle_error("socket");

    serv_addr = getaddr(host, port);

    on = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        handle_error("setsockopt");

    if (bind(sock, (struct sockaddr *) serv_addr->ai_addr, serv_addr->ai_addrlen) < 0)
        handle_error("bind");

    if(listen(sock, 5) < 0)
        handle_error("listen");

    freeaddr(serv_addr);

    return sock;
}

int accept_one(int sock)
{
    int new_sock;
    socklen_t socklen;
    struct sockaddr_in peer_addr;

    socklen = sizeof(peer_addr);
    if((new_sock = accept(sock, (struct sockaddr *) &peer_addr, &socklen)) < 0)
        handle_error("accept");

    return new_sock;
}

int sendall(int sock, unsigned char * buffer, int n)
{
    int tmp;

    while(n)
    {
        tmp = send(sock, buffer, n, 0);
        if(tmp < 0 && errno != EINTR){
            perror("send");
            return -1;
        }
        else {
            buffer += tmp;
            n -= tmp;
        }
    }

    return 0;
}

int recv_data(int sock, unsigned char * buffer, int n)
{
    int res;

    do {
        res = recv(sock, buffer, n, 0);
    } while(res <= 0 && errno == EINTR);

    return res;
}

int recvall(int sock, unsigned char * buffer, int n)
{
    int tmp;

    while(n)
    {
        tmp = recv(sock, buffer, n, 0);
        if(tmp == 0)
            return tmp;
        else if(tmp < 0 && errno != EINTR)
            return tmp;
        else if(tmp > 0) {
            buffer += tmp;
            n -= tmp;
        }
    }

    return 1;
}

void set_async(int sock)
{
    /* On get les options du fd */
    int flag = fcntl(sock, F_GETFL, 0);
    if(flag < 0){
        handle_error("fcntl F_GETFL");
    } else {
        /* On rajoute l'option asynchrone */
        flag |= O_NONBLOCK;
    }
    /* On met les changements */
    if(fcntl(sock, F_SETFL, flag) < 0)
        handle_error("fcntl F_SETFL");
}

void close_one(int sock)
{
    close(sock);
}
