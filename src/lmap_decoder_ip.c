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

    $Header$
*/

#include <lmap.h>
#include <lmap_decode.h>
#include <lmap_inet.h>
#include <lmap_addr.h>

/* globals */

struct ip_header {
#ifndef WORDS_BIGENDIAN
   u_int8   ihl:4;
   u_int8   version:4;
#else 
   u_int8   version:4;
   u_int8   ihl:4;
#endif
   u_int8   tos;
   u_int16  tot_len;
   u_int16  id;
   u_int16  frag_off;
   u_int8   ttl;
   u_int8   protocol;
   u_int16  check;
   u_int8   saddr[IP_ADDR_LEN];
   u_int8   daddr[IP_ADDR_LEN];
/*The options start here. */
};


/* protos */

FUNC_DECODER(decode_ip);
void ip_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init ip_init(void)
{
   add_decoder(NET_LAYER, LL_TYPE_IP, decode_ip);
   register_stat("IP");
}


FUNC_DECODER(decode_ip)
{
   struct ip_header *ip;
   struct ip_addr ipa_src, ipa_dst;
   char tmp[IP_ASCII_ADDR_LEN];

   update_stat("IP", 1);
   
   ip = (struct ip_header *)DECODE_DATA;
  
   DECODED_LEN = ip->ihl * 4;

   
   USER_MSG("IP : 0x%04x bytes\n%s", 
                   DECODE_DATALEN, 
                   hex_format(DECODE_DATA, DECODED_LEN));
   
   
   init_addr(&ipa_src, AF_INET, 4, (char *)&ip->saddr);
   print_addr(&ipa_src, tmp, IP_ASCII_ADDR_LEN);
   USER_MSG(" --> source  %s", tmp);
   
   init_addr(&ipa_dst, AF_INET, 4, (char *)&ip->daddr);
   print_addr(&ipa_dst, tmp, IP_ASCII_ADDR_LEN);
   USER_MSG(" --> dest    %s", tmp);

   USER_MSG(" --> proto   0x%02x\n", ip->protocol);

   if (ip->ihl * 4 != sizeof(struct ip_header))
      USER_MSG(" --> IP OPTIONS PRESENT (%d byte)\n", (ip->ihl * 4) - sizeof(struct ip_header));
      
   return get_decoder(PROTO_LAYER, ip->protocol);
}

/* EOF */

// vim:ts=3:expandtab

