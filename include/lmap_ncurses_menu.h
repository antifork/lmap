
#ifndef LMAP_NCURSES_MENU_H
#define LMAP_NCURSES_MENU_H

#include <lmap_ncurses.h>

extern WINDOW * menu_create(void);
extern void menu_refresh(WINDOW *w_menu);
extern void menu_destroy(WINDOW **w_menu);
extern void show_statistics(void);
extern int menu_event(WINDOW *w_menu, int key);

#endif

/* EOF */

// vim:ts=3:expandtab

