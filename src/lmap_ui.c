/*
    lmap -- user interface stuff

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header$
*/

#include <lmap.h>
#include <lmap_ui.h>

#include <stdarg.h>
#include <pthread.h>

/* globals */

struct ui_env {
   void (*init)(void);
   void (*start)(void);
   void (*cleanup)(void);
   void (*msg)(const char *fmt, va_list ap);
};

/* default hook */

static struct ui_env ui = {
   .init = &do_nothing,
   .start = &do_nothing,
   .cleanup = &do_nothing,
   .msg = (void (*)(const char *, va_list))&vprintf,
};

/* globa mutex on interface */

pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;

/* protos... */

void ui_init(void);
void ui_start(void);
void ui_cleanup(void);
void ui_msg(const char *fmt, ...);
void ui_register(struct ui_ops *ops);


/*******************************************/

/* called to initialize the user interface */

void ui_init(void)
{
   DEBUG_MSG("ui_init");

   ui.init();
}

/* called to run the user interface */

void ui_start(void)
{
   DEBUG_MSG("ui_start");

   ui.start();
}

/* called to end the user interface */

void ui_cleanup(void)
{
   DEBUG_MSG("ui_cleanup");

   ui.cleanup();
}

/* 
 * this function is used to display a message to 
 * the user.
 * a user interface MUST implement this as a window
 * displaying system messages
 */

void ui_msg(const char *fmt, ...)
{
   va_list ap;

   va_start(ap, fmt);
   ui.msg(fmt, ap);
   va_end(ap);

}


/*
 * register the function pointer for
 * the user interface.
 * a new user interface MUST implement this 
 * three function and use this function 
 * to hook in the right place.
 */

void ui_register(struct ui_ops *ops)
{
        
   ON_ERROR(ops->ui_init, NULL, "BUG: ui_init is equal to NULL");
   ui.init = ops->ui_init;
   
   ON_ERROR(ops->ui_cleanup, NULL, "BUG: ui_cleanup is equal to NULL");
   ui.cleanup = ops->ui_cleanup;
   
   ON_ERROR(ops->ui_start, NULL, "BUG: ui_start is equal to NULL");
   ui.start = ops->ui_start;
        
   ON_ERROR(ops->ui_msg, NULL, "BUG: ui_msg is equal to NULL");
   ui.msg = ops->ui_msg;
   
}


/* EOF */

// vim:ts=3:expandtab

