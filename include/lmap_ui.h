
#ifndef LMAP_UI_H
#define LMAP_UI_H

#include <stdarg.h>

struct ui_ops {
   void (*ui_init)(void);
   void (*ui_start)(void);
   void (*ui_cleanup)(void);
   void (*ui_msg)(const char *msg);
};

extern void ui_init(void);
extern void ui_start(void);
extern void ui_cleanup(void);
extern void ui_msg(const char *fmt, ...);
extern int ui_msg_flush(int max);
extern void ui_register(struct ui_ops *ops);

#define USER_MSG(x, args...) ui_msg(x, ## args )


#endif

/* EOF */

// vim:ts=3:expandtab

