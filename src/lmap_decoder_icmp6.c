/*
    lmap -- ICMPv6 decoder module

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

struct icmp6_header {
   u_int8   type;     /* message type */
   u_int8   code;     /* type sub-code */
   u_int16  checksum;
   union {
      u_int32 mtu;
      struct {
         u_int16 id;
         u_int16 seq;
      } echo;
   } u; 
};


/* protos */

FUNC_DECODER(decode_icmp6);
void icmp6_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init icmp6_init(void)
{
   add_decoder(PROTO_LAYER, LN_TYPE_ICMP6, decode_icmp6);
   register_stat("ICMP");
}


FUNC_DECODER(decode_icmp6)
{
   struct icmp6_header *icmp6;

   update_stat("ICMP", 1);
   
   icmp6 = (struct icmp6_header *)DECODE_DATA;
  
   DECODED_LEN = sizeof(struct icmp6_header);

   /* XXX - to be implemented */
   
   USER_MSG("ICMPv6 : 0x%04x bytes\n", DECODE_DATALEN); 
   USER_MSG(" --> type   0x%02x\n", icmp6->type);

      
   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

