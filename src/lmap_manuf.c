/*
    lmap -- manufacturer finterprint module

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

/*
 * $Id$
 *
 * MFDBL: mac fingerprint batabase library.
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
 *
 * All rights reserved.
 *
 * Linux needs queue macros from /usr/include/sys/queue.h (BSD)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 */

#include <lmap.h>
#include <lmap_data.h>
#include <lmap_hash.h>


#define TABBIT    10 /* 2^10 bit tab entries: 1024 SLISTS */
#define TABSIZE   (1UL<<TABBIT)
#define TABMASK   (TABSIZE-1) /* to mask fnv_1 hash algorithm */

#define LOAD_ENTRY(p,h,v) do {                                 \
   (p) = malloc (sizeof (struct entry));                       \
   ON_ERROR((p), NULL, "malloc() virtual memory exhausted");   \
   (p)->mac = *(int *)(h);                                     \
   (p)->vendor = strdup (v);                                   \
} while (0)

/* globals */

SLIST_HEAD(, entry) manuf_head[TABSIZE];

struct entry {
   unsigned int mac;
   char *vendor;
   SLIST_ENTRY(entry) entries;
};

/* protos */

void discard_macdb(void);
int manuf_init(void);
char * manuf_search(char *m);

/*****************************************/


void discard_macdb(void)
{
   struct entry *l;

   int i;

   for (i = 0; i < TABSIZE; i++) {

      while (SLIST_FIRST(&manuf_head[i]) != NULL) {
         l = SLIST_FIRST(&manuf_head[i]);
         SLIST_REMOVE_HEAD(&manuf_head[i], entries);
         free(l->vendor);
         free(l);
      }
   }

   DEBUG_MSG("discard_macdb");
   
   return;
}


int manuf_init(void)
{
   struct entry *p;

   char line[80];
   char name[80];
   char mac[4];

   FILE *f;

   int m1, m2, m3, i;

   i = 0;

   f = open_data(MAC_FINGERPRINTS, "r");

   while (fgets(line, 80, f) != 0) {

      if (sscanf(line, "%02X:%02X:%02X %80[^,\n],\n", &m1, &m2, &m3, name) != 4)
         continue;

      mac[0] = (char) (m1);
      mac[1] = (char) (m2);
      mac[2] = (char) (m3);
      mac[3] = 0;

      LOAD_ENTRY(p, mac, name);

      SLIST_INSERT_HEAD(&(manuf_head[fnv_hash(mac, 4) & TABMASK]), p, entries);

      i++;

   }

   DEBUG_MSG("manuf_init -- %d fingers loaded", i);
   
   fclose(f);

   atexit(discard_macdb);

   return i;
}



char * manuf_search(char *m)
{
   struct entry *l;
   char mac[4];

   mac[0] = *m++;
   mac[1] = *m++;
   mac[2] = *m;
   mac[3] = 0;

   SLIST_FOREACH(l, &manuf_head[fnv_hash(mac, 4) & TABMASK], entries) {
      if (l->mac == *(int *) mac)
         return (l->vendor);
   }

   return NULL;

}


/* EOF */

// vim:ts=3:expandtab

