
#ifndef LMAP_CAPTURE_H
#define LMAP_CAPTURE_H

#include <lmap_threads.h>

extern void capture_init(void);
extern void capture_close(void);
extern LMAP_THREAD_FUNC(capture);

#endif

/* EOF */

// vim:ts=3:expandtab

