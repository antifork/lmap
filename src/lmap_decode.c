/*
    lmap -- decoder module

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
#include <lmap_decode.h>
#include <lmap_threads.h>
#include <lmap_db.h>

#include <pcap.h>

/* globals */

static FUNC_DECODER_PTR(l2_decoder);

static SLIST_HEAD (, dec_entry) decoders_table;

struct dec_entry {
   int type;
   int level;
   FUNC_DECODER_PTR(decoder);
   SLIST_ENTRY (dec_entry) next;
};


/* protos */

void lmap_decode(u_char *u, const struct pcap_pkthdr *pkthdr, const u_char *pkt);
int set_L2_decoder(int dlt);
void add_decoder(int level, int type, FUNC_DECODER_PTR(decoder));
void * get_decoder(int level, int type);


/*******************************************/


void lmap_decode(u_char *u, const struct pcap_pkthdr *pkthdr, const u_char *pkt)
{
   FUNC_DECODER_PTR(decoder);
   struct bucket *b;
   void *next_decoder;
   int len;
   u_char *data;
   int datalen;
  
   lmap_thread_testcancel();
  
   USER_MSG("\n***************************************************************\n");
   USER_MSG("lmap_get_packets (one packet dispatched from pcap)");

   USER_MSG("CAPTURED: 0x%04x bytes\n", pkthdr->caplen);
   
   /* dump packet to file if specified on command line */

   if (GBL_OPTIONS->dump)
      pcap_dump((u_char *)GBL_PCAP->dump, pkthdr, pkt);
   
   /* we always start decoding from L2 */
   
   decoder = l2_decoder;
   
   data = (u_char *)pkt;
   datalen = pkthdr->caplen;
   
   /* alloc the bucket structure to be passet through decoders */
   b = db_bucket_alloc();
   
   do {
           
      lmap_thread_testcancel();
      
      /*
       * execute the decoder
       * it will return the next decoder or NULL if 
       * there isn't a decoder to be executed next.
       * the len of decoded data is also returned.
       */
           
      next_decoder = decoder(data, datalen, &len, b);
      
      /* 
       * update the amount of data left undecoded
       */
      
      datalen -= len;
      
      /*
       * if there is pending data, update the pointer
       */
      
      if (datalen > 0)
         data += len;
      
      decoder = next_decoder;
      
   } while (decoder != NULL);

   /* send the collected infos to the database */
   db_bucket_send(b);
   
   /* free the structure */
   db_bucket_free(b);
   
   return;
}


/*
 * set the L2 decoder and the pcap offset.
 * lookup the decoders_table to find wich decoder are
 * available
 */

int set_L2_decoder(int dlt)
{
   struct dec_entry *e;

   SLIST_FOREACH (e, &decoders_table, next) {
      if (e->level == 2 && e->type == dlt) {
         DEBUG_MSG("DLT = %d : decoder found !", dlt);
         l2_decoder = e->decoder;
         return ESUCCESS;
      }
      /* get the raw decoder if the options is specified */
      if (GBL_OPTIONS->raw && e->level == 2 && e->type == 0) {
         l2_decoder = e->decoder;
         return ESUCCESS;              
      }
   }

   /* error NOT FOUND */
   return -ENOTFOUND;
}

/*
 * add a decoder to the decoders table 
 */

void add_decoder(int level, int type, FUNC_DECODER_PTR(decoder))
{
   struct dec_entry *e;

   e = calloc(1, sizeof(struct dec_entry));
   ON_ERROR(e, NULL, "can't allocate memory");

   e->level = level;
   e->type = type;
   e->decoder = decoder;

   SLIST_INSERT_HEAD (&decoders_table, e, next); 
}

/*
 * get a decoder from the decoders table 
 */

void * get_decoder(int level, int type)
{
   struct dec_entry *e;

   SLIST_FOREACH (e, &decoders_table, next) {
      if (e->level == level && e->type == type)
         return (void *)e->decoder;
   }

   DEBUG_MSG("L%d 0x%04x not found !!", level, type);
   return NULL;
}


/* EOF */

// vim:ts=3:expandtab

