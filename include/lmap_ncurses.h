
#ifndef LMAP_NCURSES_H
#define LMAP_NCURSES_H

#include <lmap_ui.h>

#include <ncurses.h>

struct normal_window {
   WINDOW *win;         /* the windo handler */
   int lines;           /* lines displayed */
   int cols;            /* cols displayed */
   int x;               /* x coordinate */
   int y;               /* y coordinate */
   char *title;         /* the window title */
};

struct scrolling_window {
   WINDOW *win;          /* inner scrollable pad */
   WINDOW *out;         /* outer border win */
   int y_scroll;        /* scrolling position */
   int y_max;           /* max virtual lines buffer */
   int lines;           /* lines displayed */
   int cols;            /* cols displayed */
   int x;               /* x coordinate */
   int y;               /* y coordinate */
   char *title;         /* the window title */
};

typedef struct scrolling_window LMAP_SCROLLWIN;
typedef struct normal_window LMAP_WIN;

/* normal windows */

extern LMAP_WIN *lmap_newwin(int lines, int cols, int y, int x, char *title);
extern void lmap_redrawwin(LMAP_WIN *win, int focus);
extern void lmap_delwin(LMAP_WIN **win);

/* scrollable windows */

extern LMAP_SCROLLWIN *lmap_newscrollwin(int lines, int cols, int y, int x, char *title, int maxlines);
extern void lmap_redrawscrollwin(LMAP_SCROLLWIN *win, int focus);
extern void lmap_scroll(LMAP_SCROLLWIN *win, int delta);
extern void lmap_delscrollwin(LMAP_SCROLLWIN **win);


#define W(x)   (x)->win

enum {
   C_BORDER = 1,
   C_TITLE  = 2,
   C_MENU   = 3,
   C_FOCUS  = 4,
};
   
#define SAFE_WREFRESH(x)   do { wrefresh(x); } while(0)

#define SAFE_WIN_REFRESH(x)   do { wrefresh(x->win); } while(0)

#define SAFE_SCROLL_REFRESH(sx) do {  \
   pnoutrefresh(sx->win, sx->y_scroll, 0, sx->y + 1, sx->x + 1, sx->y + sx->lines - 2, sx->cols - 1 ); \
   wnoutrefresh(sx->out);              \
   doupdate();                         \
} while(0)


#define POLL_WGETCH(x, y)   do {    \
   struct pollfd poll_fd = {        \
      .fd = 0,                      \
      .events = POLLIN,             \
   };                               \
   poll(&poll_fd, 1, 1);            \
   if (poll_fd.revents & POLLIN)    \
      x = wgetch(y);                \
   else                             \
      usleep(1000);                 \
} while(0)


/* some extra key definitios */

enum {
   KEY_TAB    = '\t',    
   KEY_RETURN = 10,
   KEY_CTRL_L = 12,
};


#endif

/* EOF */

// vim:ts=3:expandtab

