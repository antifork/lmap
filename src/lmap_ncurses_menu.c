/*
    lmap -- ncurses MENU implementation

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

/* global vars */

static int stats = -1;
static WINDOW *w_stat = NULL;

/* protos */

WINDOW * menu_create(void);
void menu_refresh(WINDOW *w_menu);
void menu_destroy(WINDOW **w_menu);
void show_statistics(void);
static void refresh_statistics(void);
int menu_event(WINDOW *w_menu, int *key);

/*******************************************/

/* create the menu */

WINDOW * menu_create(void)
{
   WINDOW *w_menu;
 
   DEBUG_MSG("menu_create");
   
   /* the upper menu */
   w_menu = newwin(1, COLS, 0, 0);
   wattrset(w_menu, COLOR_PAIR(C_MENU));
   whline(w_menu, ' ', COLS);
   
   keypad(w_menu, TRUE);
   nodelay(w_menu, TRUE); 
   
   wattron(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, 0, "<F1>");     
   wattroff(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, 5, "Info");
  
   wattron(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, 10, "<F2>");     
   wattroff(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, 15, "Topology");
   
   wattron(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, COLS - 26, "<F12>");     
   wattroff(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, COLS - 20, "Packets: %8d ", get_stat("TOTAL") );
   
   SAFE_WREFRESH(w_menu);

   return w_menu;
}

/* redraw the menu */

void menu_refresh(WINDOW *w_menu)
{
   
   /* if total packet count hasn't changed, do nothing */
   if (stats == get_stat("TOTAL")) {
      return;
   }
   
   wattroff(w_menu, A_REVERSE);
   mvwprintw(w_menu, 0, COLS - 20, "Packets: %8d ", get_stat("TOTAL") );
   
   SAFE_WREFRESH(w_menu);
   stats = get_stat("TOTAL");
   refresh_statistics();
   
}

/* destroy the menu window */

void menu_destroy(WINDOW **w_menu)
{
   DEBUG_MSG("menu_destroy");
        
   werase(*w_menu);
   werase(w_stat);
   SAFE_WREFRESH(*w_menu);
   SAFE_WREFRESH(w_stat);
   delwin(*w_menu);
   delwin(w_stat);
   *w_menu = NULL;
   w_stat = NULL;
}


/* create and destroy the statistics window */

void show_statistics(void)
{

   if (w_stat != NULL) {
      werase(w_stat);
      SAFE_WREFRESH(w_stat);
      delwin(w_stat);
      w_stat = NULL;
      return;
   }
   
   w_stat = newwin(get_stat_count()+2, 20, 1, COLS - 20);

   wattron(w_stat, COLOR_PAIR(C_TITLE));
                   
   wborder(w_stat, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
                   ACS_ULCORNER, ACS_URCORNER,
                   ACS_LLCORNER, ACS_LRCORNER);

   wattroff(w_stat, COLOR_PAIR(C_TITLE));
   
   refresh_statistics(); 
}


void refresh_statistics(void)
{
   int i = 1;
   char *type;
   u_int32 num;
   
   /* if statistics are not displayed, do nothing */
   if (w_stat == NULL)
      return;
 
   /* 
    * get_all_stat has an internal counter...
    * always run until it returns 0
    */
   
   while (get_all_stats(&type, &num)) {
      mvwprintw(w_stat, i++, 2, "%5s  : %8d", type, num);
      SAFE_FREE(type);
   }
   
   SAFE_WREFRESH(w_stat);
}

/*
 * handle a key event for the menu.
 * return ESUCCESS if handled
 *       -ENOTHANDLED if not in menu
 */

int menu_event(WINDOW *w_menu, int *key)
{
   
   switch(*key) {
      case KEY_F(1):    *key = 1;            break;
      case KEY_F(2):    *key = 2;            break;
      case KEY_F(12):   show_statistics();   break;
      case 'q':         USER_MSG("Use capital Q to exit..."); break;
      case 'Q':         *key = 0;            break;
      default:          return -ENOTHANDLED; break;
   }
        
   return ESUCCESS;
}

/* EOF */

// vim:ts=3:expandtab

