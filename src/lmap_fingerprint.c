/*
    lmap -- passive TCP finterprint module

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
#include <lmap_data.h>
#include <lmap_hash.h>
#include <lmap_fingerprint.h>

#define TABBIT    7 /* 2^7 bit tab entries: 128 SLISTS */
#define TABSIZE   (1UL<<TABBIT)
#define TABMASK   (TABSIZE-1) /* to mask fnv_1 hash algorithm */

#define LOAD_ENTRY(p,h,v) do {                                 \
   (p) = malloc (sizeof (struct entry));                       \
   ON_ERROR((p), NULL, "malloc() virtual memory exhausted");   \
   memcpy((p)->finger, h, FINGER_LEN);                         \
   (p)->os = strdup (v);                                       \
} while (0)

/* globals */

SLIST_HEAD(, entry) finger_head[TABSIZE];

struct entry {
   char finger[FINGER_LEN];
   char *os;
   SLIST_ENTRY(entry) entries;
};

/* protos */

void fingerprint_discard(void);
int fingerprint_init(void);
char * fingerprint_search(char *f);

/*****************************************/


void fingerprint_discard(void)
{
   struct entry *l;

   int i;

   for (i = 0; i < TABSIZE; i++) {

      while (SLIST_FIRST(&finger_head[i]) != NULL) {
         l = SLIST_FIRST(&finger_head[i]);
         SLIST_REMOVE_HEAD(&finger_head[i], entries);
         free(l->os);
         free(l);
      }
   }

   DEBUG_MSG("fingerprint_discard");
   
   return;
}


int fingerprint_init(void)
{
   struct entry *p;
   
   int i;

   char line[128];
   char os[OS_LEN+1];
   char finger[FINGER_LEN+1];
   char *ptr;

   FILE *f;

   i = 0;

   f = open_data(TCP_FINGERPRINTS, "r");

   while (fgets(line, 128, f) != 0) {
      
      if ( (ptr = strchr(line, '#')) )
         *ptr = 0;

      /*  skip 0 length line */
      if (!strlen(line))  
         continue;
        
      strlcpy(finger, line, FINGER_LEN);
      strlcpy(os, line, OS_LEN);

      LOAD_ENTRY(p, finger, os);

      SLIST_INSERT_HEAD(&(finger_head[fnv_32(finger, FINGER_LEN) & TABMASK]), p, entries);

      i++;

   }

   DEBUG_MSG("fingerprint_init -- %d fingers loaded", i);
   
   fclose(f);

   atexit(fingerprint_discard);

   return i;
}



char * fingerprint_search(char *f)
{
   struct entry *l;

   SLIST_FOREACH(l, &finger_head[fnv_32(f, FINGER_LEN) & TABMASK], entries) {
      if (!memcmp(l->finger, f, FINGER_LEN))
         return (l->os);
   }

   return NULL;

}


/* EOF */

// vim:ts=3:expandtab

