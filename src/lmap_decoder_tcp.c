/*
    lmap -- TCP decoder module

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


/* globals */

struct tcp_header {
   u_int16  sport;      /* source port */
   u_int16  dport;      /* destination port */
   u_int32  seq;        /* sequence number */
   u_int32  ack;        /* acknowledgement number */
#ifndef WORDS_BIGENDIAN
   u_int8   x2:4;       /* (unused) */
   u_int8   off:4;      /* data offset */
#else
   u_int8   off:4;      /* data offset */
   u_int8   x2:4;       /* (unused) */
#endif
   u_int8   flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
   u_int16  win;        /* window */
   u_int16  sum;        /* checksum */
   u_int16  urp;        /* urgent pointer */
};


/* protos */

FUNC_DECODER(decode_tcp);
void tcp_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init tcp_init(void)
{
   add_decoder(PROTO_LAYER, LN_TYPE_TCP, decode_tcp);
   register_stat("TCP");
}


FUNC_DECODER(decode_tcp)
{
   struct tcp_header *tcp;

   update_stat("TCP", 1);
   
   tcp = (struct tcp_header *)DECODE_DATA;

   DECODED_LEN = (tcp->off * 4);

   /* source and dest port */
   BUCKET->L4->port_src = tcp->sport;
   BUCKET->L4->port_dst = tcp->dport;
  
   /* this is TCP */
   BUCKET->L4->proto = htons(LN_TYPE_TCP);

#if 0
   if (tcp->off * 4 != sizeof(struct tcp_header))
      USER_MSG(" --> TCP OPTIONS PRESENT (%d bytes)\n", 
                      (tcp->off * 4) - sizeof(struct tcp_header) );
   
   USER_MSG(" --> data    %d bytes\n", DECODE_DATALEN - DECODED_LEN);
#endif

   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

