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

void db_bucket_send(struct bucket *p)
{
   USER_MSG("db_bucket_send -- NOT YET IMPLEMENTED\n");

   USER_MSG("L2 INFO\n");
   SAFE_FREE(p->L2);
   
   USER_MSG("L3 INFO\n");
   SAFE_FREE(p->L3);
   
   USER_MSG("L4 INFO\n");
   SAFE_FREE(p->L4);
   
   USER_MSG("L5 INFO\n");
   SAFE_FREE(p->L5);
}


/* EOF */

// vim:ts=3:expandtab
 
