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

*/

#include <lmap.h>

/* NOTES: 
 * struct stat_env is defined in include/lmap_globals.h
 * IDs should always be uppercase for their use in interface printing 
 */

/* prototypes */

struct stat_env *get_stathead(void);
void register_stat(const char *id);
u_int16 get_max_id_len(void);
u_int32 get_stat_count(void);
void update_stat(const char *id, const u_int32 count);
void free_stats(void);

static void create_stat(const char *id);
static struct stat_env *find_stat(const char *id);

/* global variables */

static struct stat_env stathead = 
   {"TOTAL", 0, NULL};
static u_int32 stat_entries = 0;
static u_int16 max_id_len = 0;

struct stat_env *get_stathead(void)
{
   return &stathead;
}

void register_stat(const char *id)
{
   int i;

   create_stat(id);
   stat_entries++;
   i = strlen(id);
   if (max_id_len < i) 
      max_id_len = i;
}

u_int16 get_max_id_len(void)
{
   return max_id_len;
}

u_int32 get_stat_count(void)
{
   return stat_entries;
}

void create_stat(const char *id)
{
   struct stat_env *curr = &stathead;

   while (curr->next)
      curr = curr->next;

   curr->next = (struct stat_env *) calloc(1, sizeof(struct stat_env));
   curr = curr->next;

   curr->id = (char *)calloc(1, sizeof(id));
   strncpy(curr->id, id, sizeof(id));
   curr->next = NULL;
}
   
void update_stat(const char *id, const u_int32 count)
{
   struct stat_env *curr = NULL;

   curr = find_stat(id);

   if (curr)
      curr->total += count;
   stathead.total += count;
}

struct stat_env *find_stat(const char *id)
{
   struct stat_env *curr = &stathead;

   while (strcmp(id, curr->id) != 0)
      curr = curr->next;

   return curr;
}

void free_stats(void)
{
   struct stat_env *curr = &stathead, *tmp = NULL;

   curr = curr->next;
   
   while (curr->next) {
      tmp = curr->next;
      SAFE_FREE(curr);
      curr = tmp;
   }
}
      
/* EOF */

// vim:ts=3:expandtab

