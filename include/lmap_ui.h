
#ifndef LMAP_UI_H
#define LMAP_UI_H

#include <stdarg.h>
#include <pthread.h>

struct ui_ops {
   void (*ui_init)(void);
   void (*ui_start)(void);
   void (*ui_cleanup)(void);
   void (*ui_msg)(const char *fmt, va_list ap);
};

extern void ui_init(void);
extern void ui_start(void);
extern void ui_cleanup(void);
extern void ui_msg(const char *fmt, ...);
extern void ui_register(struct ui_ops *ops);

#define USER_MSG(x, args...) ui_msg(x, ## args )


extern pthread_mutex_t ui_mutex;

#define UI_LOCK   pthread_mutex_lock(&ui_mutex)
#define UI_UNLOCK pthread_mutex_unlock(&ui_mutex)

#endif

/* EOF */

// vim:ts=3:expandtab

