
#ifndef LMAP_THREADS_H
#define LMAP_THREADS_H

#include <lmap_stdint.h>

struct lmap_thread {
   char *name;
   char *description;
   u_int32 id;
};

enum {
   LMAP_SELF = 0,
};

#define LMAP_THREAD_FUNC(x) void * x(void *args)
#define LMAP_THREAD_PARAM  args

extern char * lmap_thread_getname(u_int32 id);
extern char * lmap_thread_getdesc(u_int32 id);
extern void lmap_thread_register(u_int32 id, char *name, char *desc);
extern u_int32 lmap_thread_new(char *name, char *desc, void *(*function)(void *), void *args);
extern void lmap_thread_destroy(u_int32 id);
extern void lmap_thread_init(void);
extern void lmap_thread_testcancel(void);
extern void lmap_thread_kill_all(void);

#endif

/* EOF */

// vim:ts=3:expandtab

