/*
    lmap -- ncurses user interface implementation

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
#include <lmap_ncurses.h>
#include <lmap_ncurses_info.h>
#include <lmap_ncurses_topology.h>

#include <ncurses.h>

/* global vars */

static int onscreen = 0;

/* protos */

void ncurses_register(void);
static void ncurses_init(void);
static void ncurses_start(void);
static void ncurses_cleanup(void);

LMAP_WIN *lmap_newwin(int lines, int cols, int x, int y, char *title);
void lmap_redrawwin(LMAP_WIN *win, int focus);
void lmap_delwin(LMAP_WIN **win);

LMAP_SCROLLWIN *lmap_newscrollwin(int lines, int cols, int x, int y, char *title, int maxline);
void lmap_redrawscrollwin(LMAP_SCROLLWIN *win, int focus);
static void lmap_drawscroller(LMAP_SCROLLWIN *win);
void lmap_scroll(LMAP_SCROLLWIN *win, int delta);
void lmap_delscrollwin(LMAP_SCROLLWIN **win);

/*******************************************/

void __init ncurses_register(void)
{
   struct ui_ops ncurses_ui_ops = {
      .ui_init     = &ncurses_init,
      .ui_cleanup  = &ncurses_cleanup,
      .ui_start    = &ncurses_start,
      .ui_msg      = &ncurses_msg,
   };
   
   /* register the ncurses ops for the user interface */

   ui_register(&ncurses_ui_ops);
   
}

/* initialize the ncurse interface */

void ncurses_init(void)
{

   DEBUG_MSG("ncurses_init");
   
   /* ncurses initialization */
        
   initscr();

   /* 
    * the interface is initialized.
    * remember this on ncurses_cleanup
    */
   
   onscreen = 1;
   
   cbreak();
   noecho();
   nonl();
   clear();
   
   /* initialize the colors */
   
   if (has_colors()) 
      start_color();

   init_pair(C_BORDER, COLOR_WHITE, COLOR_BLACK);
   init_pair(C_TITLE, COLOR_CYAN, COLOR_BLACK);
   init_pair(C_MENU, COLOR_CYAN, COLOR_BLACK);
   init_pair(C_FOCUS, COLOR_YELLOW, COLOR_BLACK);
   
   /* hide the cursor */
   curs_set(0);
   
   atexit(ncurses_cleanup);
}


void ncurses_cleanup(void)
{
   /* if the screen is not initialized... */
   if (!onscreen)
      return;

   DEBUG_MSG("ncurses_cleanup");
   
   onscreen = 0;
   
   /* show the cursor */
   curs_set(1);
   
   /* clear the screen */
   
   clear();
   refresh();
   
   /* finalize ncurses */
   endwin();
   
   DEBUG_MSG("ncurses_cleanup -- cleaned !");
}


void ncurses_start(void)
{
   int next = 1;
   int (*display_screen)(void) = scr_info;
   
   DEBUG_MSG("ncurses_init");

   /* 
    * main loop...
    * till the app is running we are in this loop
    */
  
   do {
      switch (next = display_screen()) {
         
         /*
          * add here new screens
          */
         
         case 1:
            display_screen = scr_info;
            break;
         case 2:
            display_screen = scr_topology;
            break;
      }
   } while (next != 0);

   /* 
    * here we return in the main...       
    * the ui_clenup will be performed there.
    */
   
   return;
}


/* windows facilities */

LMAP_WIN *lmap_newwin(int lines, int cols, int y, int x, char *title)
{
   LMAP_WIN *win;

   DEBUG_MSG("lmap_newwin -- %s", title);
   
   win = calloc(1, sizeof(LMAP_WIN));
   ON_ERROR(win, NULL, "can't allocate memory");
   
   win->win = newwin(lines, cols, y, x);
   win->x = x;
   win->y = y;
   win->lines = lines;
   win->cols = cols;
   win->title = strdup(title);

   wborder(win->win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER,
            ACS_LLCORNER, ACS_LRCORNER);

   wmove(win->win, 0, 2);
   wattrset(win->win, COLOR_PAIR(C_TITLE));
   waddstr(win->win, " ");
   waddstr(win->win, title);
   waddstr(win->win, " ");
   wattroff(win->win, COLOR_PAIR(C_TITLE));

   scrollok(win->win, TRUE);
   
   return win;
}

/* redraw the window with or without focus */

void lmap_redrawwin(LMAP_WIN *win, int focus)
{
   if (focus)
      wattron(win->win, A_BOLD);

   werase(win->win);
   wmove(win->win, 0, 0);
   
   wattron(win->win, COLOR_PAIR(C_BORDER));
   
   wborder(win->win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER,
            ACS_LLCORNER, ACS_LRCORNER);

   if (focus) {
      wmove(win->win, 0, 0);
      wattron(win->win, COLOR_PAIR(C_FOCUS));
      waddch(win->win, ACS_LARROW);
      waddch(win->win, ACS_RARROW);
      wattroff(win->win, COLOR_PAIR(C_FOCUS));
   }
   
   wmove(win->win, 0, 2);
   wattrset(win->win, COLOR_PAIR(C_TITLE));
   waddstr(win->win, " ");
   waddstr(win->win, win->title);
   waddstr(win->win, " ");
   wattroff(win->win, COLOR_PAIR(C_TITLE));
   
   if (focus)
      wattroff(win->win, A_BOLD);
 
   SAFE_WIN_REFRESH(win);
}

/* dealloc a win */

void lmap_delwin(LMAP_WIN **win)
{
   DEBUG_MSG("lmap_delwin -- %s", (*win)->title);
        
   werase((*win)->win); 
   SAFE_WREFRESH((*win)->win); 
   delwin((*win)->win);
   SAFE_FREE((*win)->title);
   SAFE_FREE(*win);
}

/*
 * create two window: one to draw the border
 * and an internal pad to simulate the scrolling 
 * window
 */

LMAP_SCROLLWIN *lmap_newscrollwin(int lines, int cols, int y, int x, char *title, int maxlines)
{
   LMAP_SCROLLWIN *win;
 
   DEBUG_MSG("lmap_newscrollwin -- %s", title);
 
   win = calloc(1, sizeof(LMAP_SCROLLWIN));
   ON_ERROR(win, NULL, "can't allocate memory");
        
   win->win = newpad(maxlines, cols - 2);
   win->out = newwin(lines, cols, y, x);
   win->y_max = maxlines;
   win->lines = lines;
   win->cols = cols;
   win->x = x;
   win->y = y;
   win->title = strdup(title);

   scrollok(win->win, TRUE);

   /* move the cursor to the right starting point */
   
   win->y_scroll = maxlines - (lines-2);
   wmove(win->win, win->y_scroll - 1, 0);        
   
   /* draw the outer window */
   
   wattrset(win->out, COLOR_PAIR(C_BORDER));

   wborder(win->out, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER,
            ACS_LLCORNER, ACS_LRCORNER);

   wmove(win->out, 0, 2);
   wattrset(win->out, COLOR_PAIR(C_TITLE));
   waddstr(win->out, " ");
   waddstr(win->out, title);
   waddstr(win->out, " ");
   wattrset(win->out, COLOR_PAIR(C_BORDER));

   lmap_drawscroller(win);
   
   return win;
}

/* redraw the window with or without focus */

void lmap_redrawscrollwin(LMAP_SCROLLWIN *win, int focus)
{
   
   if (focus)
      wattron(win->out, A_BOLD);
 
   werase(win->out);
   wmove(win->out, 0, 0);
  
   wattron(win->out, COLOR_PAIR(C_BORDER));
   
   wborder(win->out, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
            ACS_ULCORNER, ACS_URCORNER,
            ACS_LLCORNER, ACS_LRCORNER);

   if (focus) {
      wmove(win->out, 0, 0);
      wattron(win->out, COLOR_PAIR(C_FOCUS));
      waddch(win->out, ACS_LARROW);
      waddch(win->out, ACS_RARROW);
      wattroff(win->out, COLOR_PAIR(C_FOCUS));
   }
   
   wattron(win->out, COLOR_PAIR(C_TITLE));
   wmove(win->out, 0, 2);
   waddstr(win->out, " ");
   waddstr(win->out, win->title);
   waddstr(win->out, " ");
   wattroff(win->out, COLOR_PAIR(C_TITLE));

   redrawwin(win->out);

   if (focus)
      wattroff(win->out, A_BOLD);
   
   lmap_drawscroller(win);   

   SAFE_SCROLL_REFRESH(win);
}


/* display the scroll indicator on the right */

void lmap_drawscroller(LMAP_SCROLLWIN *win)
{
   short height = (win->lines-2) * (win->lines-2) / win->y_max;
   short vpos = win->lines * win->y_scroll / win->y_max;

   wattron(win->out, COLOR_PAIR(C_TITLE));
   wattroff(win->out, A_BOLD);

   height = (height < 1) ? 1 : height;

   vpos = (vpos == 0) ? 1 : vpos;
   vpos = (vpos > (win->lines-1) - height) ? (win->lines-1) - height : vpos;
                     
   wmove(win->out, 1, win->x + win->cols - 1);
   wvline(win->out, ACS_CKBOARD, win->lines - 2);
   wattron(win->out, A_REVERSE);
   wmove(win->out, vpos, win->x + win->cols - 1);
   wvline(win->out, ' ', height);
   wattroff(win->out, A_REVERSE);
                                          
   wnoutrefresh(win->out);
}


/* scroll a window for delta lines */

void lmap_scroll(LMAP_SCROLLWIN *win, int delta)
{
   win->y_scroll += delta;
   win->y_scroll = (win->y_scroll < 0) ? 0 : win->y_scroll;
   win->y_scroll = (win->y_scroll > win->y_max - (win->lines-2) ) 
                 ? win->y_max - (win->lines-2) 
                 : win->y_scroll;
  
   lmap_drawscroller(win);   
   
   SAFE_SCROLL_REFRESH(win);
}


void lmap_delscrollwin(LMAP_SCROLLWIN **win)
{
   DEBUG_MSG("lmap_delscrollwin -- %s", (*win)->title);
   
   werase((*win)->win); 
   SAFE_WREFRESH((*win)->win); 
   werase((*win)->out); 
   SAFE_WREFRESH((*win)->out); 
   delwin((*win)->win);
   delwin((*win)->out);
   SAFE_FREE((*win)->title);
   SAFE_FREE(*win);
}


/* EOF */

// vim:ts=3:expandtab

