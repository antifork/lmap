/*
    lmap -- ICMP decoder module

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
#include <lmap_inet.h>

/* globals */

struct icmp_header {
   u_int8   type;     /* message type */
   u_int8   code;     /* type sub-code */
   u_int16  checksum;
   union {
      struct {
         u_int16  id;
         u_int16  sequence;
      } echo;       /* echo datagram */
      u_int32     gateway; /* gateway address */
      struct {
         u_int16  unused;
         u_int16  mtu;
      } frag;       /* path mtu discovery */
   } un;
};


/* protos */

FUNC_DECODER(decode_icmp);
void icmp_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init icmp_init(void)
{
   add_decoder(PROTO_LAYER, LN_TYPE_ICMP, decode_icmp);
   register_stat("ICMP");
}


FUNC_DECODER(decode_icmp)
{
   struct icmp_header *icmp;

   update_stat("ICMP", 1);
   
   icmp = (struct icmp_header *)DECODE_DATA;
  
   DECODED_LEN = sizeof(struct icmp_header);

   /* XXX - to be implemented */
   
   USER_MSG("ICMP : 0x%04x bytes\n", DECODE_DATALEN); 
   USER_MSG(" --> type   0x%02x\n", icmp->type);

      
   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

