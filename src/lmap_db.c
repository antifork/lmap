/*
    lmap -- database module

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
#include <lmap_db.h>
#include <lmap_ui.h>

/* XXX - remove me !  (functions in bucket_send) */
#include <lmap_if.h>

/* protos */

struct bucket * db_bucket_alloc(void);
void db_bucket_free(struct bucket *);
void db_bucket_send(struct bucket *);

/*******************************************/

/*******************************************/

/* 
 * create and alloc space for a bucket used for exchanging messages 
 */

struct bucket * db_bucket_alloc(void)
{
   struct bucket *p;

   p = (struct bucket *) calloc(1, sizeof(struct bucket));
   ON_ERROR(p, NULL, "can't allocate bucket");

   p->L2 = (struct L2_info *) calloc(1, sizeof(struct L2_info));
   p->L3 = (struct L3_info *) calloc(1, sizeof(struct L3_info));
   p->L4 = (struct L4_info *) calloc(1, sizeof(struct L4_info));
   p->L5 = (struct L5_info *) calloc(1, sizeof(struct L5_info));

   if ( p->L2 == NULL || p->L3 == NULL || p->L4 == NULL || p->L5 == NULL )
      ERROR_MSG("can't allocate memory");
      
   return p;
}

/* 
 * dealloc space of bucket, keeping the content allocated
 */
 
void db_bucket_free(struct bucket *p)
{
   SAFE_FREE(p);
}

/* 
 * send the bucket to the database */

void db_bucket_send(struct bucket *b)
{
   char tmp[50];     /* XXX -- ONLY FOR DEBUGGING PURPOSE */
   char details[50]; /* XXX -- ONLY FOR DEBUGGING PURPOSE */
   
   USER_MSG("db_bucket_send -- NOT YET IMPLEMENTED\n");

   USER_MSG("\n--- L2 INFO ---\n");
   eth_addr_ntoa(b->L2->mac_src, tmp);
   USER_MSG(" --> source  %s", tmp);
   eth_addr_ntoa(b->L2->mac_dst, tmp);
   USER_MSG(" --> dest    %s", tmp);
   USER_MSG(" --> ESSID   %s", b->L2->ESSID);
   SAFE_FREE(b->L2->ESSID);
   SAFE_FREE(b->L2);
   
   USER_MSG("\n--- L3 INFO ---\n");
   ip_addr_ntoa(&b->L3->ip_src, tmp);
   ip_addr_details(&b->L3->ip_src, details);
   USER_MSG(" --> source  %s %s", tmp, details);
   ip_addr_ntoa(&b->L3->ip_dst, tmp);
   ip_addr_details(&b->L3->ip_dst, details);
   USER_MSG(" --> dest    %s %s", tmp, details);
   USER_MSG(" --> proto   0x%04x", ntohs(b->L3->proto));
   USER_MSG(" --> ttl     %d", b->L3->ttl);
   SAFE_FREE(b->L3);
   
   USER_MSG("\n--- L4 INFO ---\n");
   USER_MSG(" --> source  %d", ntohs(b->L4->port_src));
   USER_MSG(" --> dest    %d", ntohs(b->L4->port_dst));
   USER_MSG(" --> proto   0x%04x", ntohs(b->L4->proto));
   USER_MSG(" --> finger  %s\n", b->L4->fingerprint);
   SAFE_FREE(b->L4->fingerprint);
   SAFE_FREE(b->L4);
   
//   USER_MSG("L5 INFO\n");
//   SAFE_FREE(b->L5);
}


/* EOF */

// vim:ts=3:expandtab
 
