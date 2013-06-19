#include "thread.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

thread_id new_thread(void *(* target)(void *), void * arg)
{
    thread_id res;
    if(pthread_create(&res, NULL, target, arg) != 0)
        handle_error("pthread_create");

    if(pthread_detach(res) < 0)
        handle_error("pthread_detach");

    return res;
}

int kill_thread(thread_id id)
{
    return pthread_cancel(id);
}

void join_thread(thread_id id)
{
    if(pthread_join(id, NULL) != 0)
        handle_error("pthread_join");
}

void exit_thread()
{
    pthread_exit(NULL);
}
