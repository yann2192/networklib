#ifndef __NODE__
#define __NODE__

#include "../network/network.h"
#include "../other/data.h"
#include "../other/thread.h"
#include "../other/utils.h"

#define MAX_EVENTS 128

struct _job_list
{
    ID id;
    int n;
    char type;
    thread_id thread;
    struct _job_list * next;
    struct _job_list * prev;
};
typedef struct _job_list job_list;

void close_connection(client_s * c);
void the_loop(void * arg);

void create_mapper(client_s * c);
void create_reducer(client_s * c);
void get_data(client_s * c);
void get_code_reducer(client_s * c);
void get_code_mapper(client_s * c);
void get_requete(client_s * c);
void clean_mapper(client_s * c);
void clean_reducer(client_s * c);
char * gen_filename(ID id, char * type);
void save_data(ID id, char * type, unsigned char * buffer, int size);
int compile_reducer(ID id);
int compile_mapper(ID id);
int my_system(char * command);

int launch_reducer(ID id);
void * reducer_task(void * arg);

void add_reducer(job_list * job);
job_list * get_reducer(ID id);
void delete_reducer(ID id);

void add_mapper(job_list * job);
job_list * get_mapper(ID id);
void delete_mapper(ID id);

#endif
