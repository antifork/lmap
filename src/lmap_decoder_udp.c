/*
    lmap -- IP decoder module

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
}


FUNC_DECODER(decode_udp)
{
   struct udp_header *udp;

   udp = (struct udp_header *)DECODE_DATA;

   DECODED_LEN = sizeof(struct udp_header);
   
   DEBUG_MSG("UDP : 0x%04x bytes\n%s\n", 
                   DECODE_DATALEN, 
                   hex_format(DECODE_DATA, DECODED_LEN));
   
   DEBUG_MSG(" --> source  %d", ntohs(udp->sport));
   DEBUG_MSG(" --> dest    %d", ntohs(udp->dport));

   DEBUG_MSG(" --> data    %d bytes\n", ntohs(udp->ulen) - DECODED_LEN);
   
   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

