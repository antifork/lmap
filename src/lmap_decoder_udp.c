/*
    lmap -- UDP decoder module

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

struct udp_header {
   u_int16  sport;           /* source port */
   u_int16  dport;           /* destination port */
   u_int16  ulen;            /* udp length */
   u_int16  sum;             /* udp checksum */
};

/* protos */

FUNC_DECODER(decode_udp);
void udp_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init udp_init(void)
{
   add_decoder(PROTO_LAYER, LN_TYPE_UDP, decode_udp);
   register_stat("UDP");
}


FUNC_DECODER(decode_udp)
{
   struct udp_header *udp;

   update_stat("UDP", 1);
   
   udp = (struct udp_header *)DECODE_DATA;

   DECODED_LEN = sizeof(struct udp_header);

   /* source and dest port */
   BUCKET->L4->port_src = udp->sport;
   BUCKET->L4->port_dst = udp->dport;

   /* this is UDP */
   BUCKET->L4->proto = htons(LN_TYPE_UDP);

   /* return the next decoder */
   return get_decoder(APP_LAYER, ntohs(udp->dport));
}

/* EOF */

// vim:ts=3:expandtab

