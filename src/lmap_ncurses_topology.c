/*
    lmap -- ncurses TOPOLOGY implementation

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
#include <lmap_ncurses_menu.h>

#include <poll.h>

/* protos */

int scr_topology(void);

/*******************************************/

/* 
 * handle the "topology" screen
 * returns the next screen to be displayed
 */

int scr_topology(void)
{
   int inputkey = 0; 
   WINDOW *w_menu;
  
   /* the upper menu */
   w_menu = menu_create();
   SAFE_WREFRESH(w_menu);

   /* loop inside this screen */
   
   do {
      inputkey = 0xff;  /* unused value */

      POLL_WGETCH(inputkey, w_menu);
      
      menu_refresh(w_menu);

      menu_event(w_menu, &inputkey);
      
      switch(inputkey) {
         case KEY_TAB:
            break;
      }
 
      /* flush the messages */
      ui_msg_flush(-1);
      
   } while ( inputkey > 12 );
  
   /* clean up the screen and return */
   
   menu_destroy(&w_menu);
   
   DEBUG_MSG("scr_topology exits with code %d", inputkey);
   
   return inputkey;        
}


/* EOF */

// vim:ts=3:expandtab

