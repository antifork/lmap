/*
    lmap -- statistics management

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

/* NOTES: 
 * TYPEs should always be uppercase for their use in interface printing 
 */

/* global variables */

struct stat_list {
   char *type;
   u_int32 num;
   LIST_ENTRY (stat_list) next;
};
   
LIST_HEAD(, stat_list) stat_list_head;

static u_int32 stat_entries = 0;

/* prototypes */

void register_stat(const char *type);
u_int32 get_stat_count(void);
void update_stat(const char *type, const u_int32 count);
u_int32 get_stat(const char *type);
void free_stats(void);

/***************************************/


void register_stat(const char *type)
{
   struct stat_list *newelem, *current;

   newelem = (struct stat_list *) calloc(1, sizeof(struct stat_list));
   ON_ERROR(newelem, NULL, "can't allocate memory");

   newelem->type = strdup(type);
   newelem->num = 0;

   LIST_FOREACH(current, &stat_list_head, next) {
      if (!strcmp(current->type, type)) {
         LIST_REPLACE(current, newelem, next);
         SAFE_FREE(current);
         return;
      }
   }
   
   LIST_INSERT_HEAD(&stat_list_head, newelem, next);
   
   stat_entries++;
}

u_int32 get_stat_count(void)
{
   return stat_entries;
}

   
void update_stat(const char *type, const u_int32 count)
{
   struct stat_list *current;

   LIST_FOREACH(current, &stat_list_head, next) {
      if (!strcmp(current->type, type))
         current->num += count;
   }
   
}


u_int32 get_stat(const char *type)
{
   struct stat_list *current;

   LIST_FOREACH(current, &stat_list_head, next) {
      if (!strcmp(current->type, type))
         return current->num;
   }
   
   return 0; 
}

void free_stats(void)
{
   struct stat_list *curr;

   DEBUG_MSG("free_stats");
   
   while (LIST_FIRST(&stat_list_head) != NULL) {
      curr = LIST_FIRST(&stat_list_head);
      LIST_REMOVE(curr, next);
      SAFE_FREE(curr->type);
      SAFE_FREE(curr);
   }
}
      
/* EOF */

// vim:ts=3:expandtab

