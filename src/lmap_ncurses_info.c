/*
    lmap -- ncurses INFO implementation

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
#include <lmap_if.h>
#include <lmap_ui.h>
#include <lmap_ncurses.h>
#include <lmap_ncurses_menu.h>

#include <poll.h>

#define SYS_LINES 10

/* global vars */

static LMAP_SCROLLWIN *w_sys;

/* protos */

int scr_info(void);
void ncurses_msg(const char *fmt, va_list ap);
static void print_ifaceconfig(LMAP_WIN *win);
static void print_suggconfig(LMAP_WIN *win);

/*******************************************/

/* 
 * handle the "info" screen
 * returns the next screen to be displayed
 */

int scr_info(void)
{
   int inputkey = 0; 
   WINDOW *w_menu;
   LMAP_WIN *w_info, *w_sugg;
  
   /* the upper menu */
   w_menu = menu_create();
   SAFE_WREFRESH(w_menu);

   /* interface info window */

   w_info = lmap_newwin(6, 55, 2, 0, "ACTUAL Network Interface Settings");
   print_ifaceconfig(w_info);
   SAFE_WIN_REFRESH(w_info);
   
   /* suggested config */
   
   w_sugg = lmap_newwin(6, 55, 8, 0, "SUGGESTED Network Interface Settings");
   print_suggconfig(w_sugg);
   SAFE_WIN_REFRESH(w_sugg);
   
   /* the system message window */
   w_sys = lmap_newscrollwin(SYS_LINES, COLS, LINES - SYS_LINES, 0, "System messages", 150);
   SAFE_SCROLL_REFRESH(w_sys);

  
   /* loop inside this screen */
   
   do {
      inputkey = 0;

      POLL_WGETCH(inputkey, w_menu);
      
      menu_refresh(w_menu);

      menu_event(w_menu, inputkey);
      
      switch(inputkey) {
         case KEY_DOWN:    lmap_scroll(w_sys, +1); break;
         case KEY_UP:      lmap_scroll(w_sys, -1); break;
         case KEY_NPAGE:   lmap_scroll(w_sys, +SYS_LINES); break;
         case KEY_PPAGE:   lmap_scroll(w_sys, -SYS_LINES); break;
         case KEY_TAB:
            lmap_redrawscrollwin(w_sys, 1);
            USER_MSG("TAB: redrawing window");
            break;
         case 'q':         USER_MSG("Use capital Q to exit..."); break;
      }
      
   } while (inputkey != 'Q');
   
   /* clean up the screen and return */
   
   lmap_delscrollwin(&w_sys);
   lmap_delwin(&w_info);
   lmap_delwin(&w_sugg);
   menu_destroy(&w_menu);
  
   return 0;        
}

/*
 * handler for the USER_MSG implementation
 */

void ncurses_msg(const char *fmt, va_list ap)
{
   /* 
    * if the window doesn't exist 
    */
   if (w_sys == NULL) 
      return;
 
   wprintw(W(w_sys), "\n");
   vwprintw(W(w_sys), (char *)fmt, ap);
   SAFE_SCROLL_REFRESH(w_sys);
   
}

/* retrives and print actual iface configuration */

void print_ifaceconfig(LMAP_WIN *win)
{
   u_int32 tmp;
   struct ip_addr ipa, ipa6, netmask, gw_addr, dns_addr;
   u_int8 ll_addr[ETH_ADDR_LEN];
   char ip_ascii[IP_ASCII_ADDR_LEN];
   char ip6_ascii[IP6_ASCII_ADDR_LEN];
   char net_ascii[IP_ASCII_ADDR_LEN];
   char ll_ascii[ETH_ASCII_ADDR_LEN];
   char gw_ascii[IP_ASCII_ADDR_LEN];
   char dns_ascii[IP_ASCII_ADDR_LEN];
   
   /* get and convert iface infos */
   if (get_iface_ip(GBL_OPTIONS->iface, &ipa, AF_INET) == ESUCCESS) {
      ip_addr_ntoa(&ipa, ip_ascii);
   } else {
      sprintf(ip_ascii, "(none)");
   }     
      
   if (get_iface_ip(GBL_OPTIONS->iface, &ipa6, AF_INET6) == ESUCCESS) {
      ip_addr_ntoa(&ipa6, ip6_ascii);
   } else {
      sprintf(ip6_ascii, "(none)");
   }
      
   /* XXX remove me! */
   DEBUG_MSG("inet6_addr: %s", ip6_ascii);

   
   if (get_iface_mask(GBL_OPTIONS->iface, &tmp) == ESUCCESS) {
      ip_addr_init(&netmask, AF_INET, (char *)&tmp);
      ip_addr_ntoa(&netmask, net_ascii);
   } else {
      sprintf(net_ascii, "(none)");
   }

   if (get_default_gw(&tmp) == ESUCCESS) {
      ip_addr_init(&gw_addr, AF_INET, (char *)&tmp);
      ip_addr_ntoa(&gw_addr, gw_ascii);
   } else {                                                                    
      sprintf(gw_ascii, "(none)");
   } 

   if (get_default_dns(&tmp) == ESUCCESS) {
      ip_addr_init(&dns_addr, AF_INET, (char *)&tmp);
      ip_addr_ntoa(&dns_addr, dns_ascii);
   } else {
      sprintf(dns_ascii, "(none)");
   }  

   get_iface_ll(GBL_OPTIONS->iface, ll_addr);
   eth_addr_ntoa(ll_addr, ll_ascii);
   
   
   /* draw the window */
   
   /* XXX remove me! */
   
   /* name */
   
   wmove(W(win), 2, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "Name : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), GBL_OPTIONS->iface);
   wattroff(W(win), A_BOLD);
  
   /* hw address */
   
   wmove(W(win), 2, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "HWaddr  : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), ll_ascii);
   wattroff(W(win), A_BOLD);

   /* ip address */
   
   wmove(W(win), 3, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "inet : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), ip_ascii);
   wattroff(W(win), A_BOLD);
   
   /* netmask */
   
   wmove(W(win), 3, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "netmask : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), net_ascii);
   wattroff(W(win), A_BOLD);

   /* default gateway */

   wmove(W(win), 4, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "GW   : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), gw_ascii);
   wattroff(W(win), A_BOLD);

   /* default dns */

   wmove(W(win), 4, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "DNS     : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), dns_ascii);
   wattroff(W(win), A_BOLD);
}


/* retrives and print actual iface configuration */

void print_suggconfig(LMAP_WIN *win)
{
   /* draw the window */
   
   /* name */
   
   wmove(W(win), 2, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "Name : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), GBL_OPTIONS->iface);
   wattroff(W(win), A_BOLD);
  
   /* hw address */
   
   wmove(W(win), 2, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "HWaddr  : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), "(none)");
   wattroff(W(win), A_BOLD);

   /* ip address */
   
   wmove(W(win), 3, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "inet : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), "(none)");
   wattroff(W(win), A_BOLD);
   
   /* netmask */
   
   wmove(W(win), 3, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "netmask : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), "(none)");
   wattroff(W(win), A_BOLD);

   /* default gateway */

   wmove(W(win), 4, 2);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "GW   : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), "(none)");
   wattroff(W(win), A_BOLD);

   /* default gateway */

   wmove(W(win), 4, 25);
   wattron(W(win), COLOR_PAIR(C_TITLE));
   waddstr(W(win), "DNS     : ");
   wattroff(W(win), COLOR_PAIR(C_TITLE));

   wattron(W(win), A_BOLD);
   waddstr(W(win), "(none)");
   wattroff(W(win), A_BOLD);

}


/* EOF */


// vim:ts=3:expandtab

