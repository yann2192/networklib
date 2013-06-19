#ifndef __NETWORK__
#define __NETWORK__

#include "sock.h"
#include "async.h"

struct _client_s
{
    int fd;
    void (*func) (void *);
    async_s * as;
    void * data;
};

typedef struct _client_s client_s;

void loop(async_s * as);

#endif
