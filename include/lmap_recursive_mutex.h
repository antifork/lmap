
#ifndef LMAP_THREADS_H
#define LMAP_THREADS_H

#include <lmap_stdint.h>
#include <pthread.h>

extern int recursive_mutex_lock(pthread_mutex_t * mutex);
extern int recursive_mutex_unlock(pthread_mutex_t * mutex);

#endif

/* EOF */

// vim:ts=3:expandtab

