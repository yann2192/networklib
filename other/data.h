#ifndef __DATA__
#define __DATA__

#include "../network/network.h"

typedef char ID[9];

/*struct _jobber_s
{
    ID id;
    int n;
    char typeS;
};
typedef struct _jobber_s jobber_s;

struct _node_s
{
    int sock;
};
typedef struct _node_s node_s;

struct _mapper_s
{
    int sock;
};
typedef struct _mapper_s mapper_s;

struct _reducer_s
{
    void * none;
};
typedef struct _reducer_s reducer_s;*/

struct _job_s
{
    ID id;
    int n;
    char typeS;
    client_s * jobber;
    client_s * reducer;
    client_s * reducer_task;
    client_s ** mappers;
};
typedef struct _job_s job_s;

#endif
