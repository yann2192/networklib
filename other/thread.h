#ifndef __THREAD__
#define __THREAD__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

typedef pthread_t thread_id;

thread_id new_thread(void *(*target)(void *), void *);
int kill_thread(thread_id);
void join_thread(thread_id);
void exit_thread();

#endif
