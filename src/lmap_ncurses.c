/*
    lmap -- ncurse user interface implementation

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

*/

#include <lmap.h>
#include <lmap_ui.h>

#include <ncurses.h>

/* global vars */

int onscreen = 0;

/* protos */

void ncurses_register(void);
void ncurses_init(void);
void ncurses_start(void);
void ncurses_cleanup(void);


/*******************************************/

void __init ncurses_register(void)
{
#if 0
   struct ui_ops ncurses_ui_ops = {
      .ui_init     = &ncurses_init,
      .ui_cleanup  = &ncurses_cleanup,
      .ui_start    = &ncurses_start,
      .ui_msg      = (void (*)(const char *, va_list))&vprintf,
   };
#endif   
   struct ui_ops ncurses_ui_ops = {
      .ui_init     = &do_nothing,
      .ui_cleanup  = &do_nothing,
      .ui_start    = &ncurses_start,
      .ui_msg      = (void (*)(const char *, va_list))&vprintf,
   };
   /* register a dummy ops for the user interface */

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

   /* hide the cursor */
   
   curs_set(0);
   
   atexit(ncurses_cleanup);
}


void ncurses_cleanup(void)
{
   DEBUG_MSG("ncurses_cleanup");
   
   if (!onscreen)
      return;

   onscreen = 0;
   
   /* show the cursor */
   curs_set(1);
   
   /* clear the screen */
   
   clear();
   refresh();
   
   /* finalize ncurses */
   endwin();

}


void ncurses_start(void)
{
   DEBUG_MSG("ncurses_init");
        
   /* XXX - to be implemented */
   while(1) sleep(1);
          
}


/* EOF */


// vim:ts=3:expandtab

