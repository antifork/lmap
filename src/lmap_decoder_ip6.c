/*
    lmap -- IPv6 decoder module

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
#include <lmap_fingerprint.h>

enum {
   IP6_HDR_LEN = 40,
};

/* globals */

struct ip6_header {
#ifndef WORDS_BIGENDIAN
   u_int8   version:4;
   u_int8   priority:4;
#else 
   u_int8   priority:4;
   u_int8   version:4;
#endif
   u_int8   flow_lbl[3];
   u_int16  payload_len;
   u_int8   next_hdr;
   u_int8   hop_limit;

   u_int8   saddr[IP6_ADDR_LEN];
   u_int8   daddr[IP6_ADDR_LEN];
   
   /* OPTIONS MAY FOLLOW */
};

/* protos */

FUNC_DECODER(decode_ip6);
void ip6_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init ip6_init(void)
{
   add_decoder(NET_LAYER, LL_TYPE_IP6, decode_ip6);
   register_stat("IP");
}


FUNC_DECODER(decode_ip6)
{
   struct ip6_header *ip6;
   int opt; /* -1 means no options defined, if 0 an option is present */
   
   update_stat("IP", 1);
   
   ip6 = (struct ip6_header *)DECODE_DATA;
  
   
   DECODED_LEN = ip6->payload_len + IP6_HDR_LEN;

   /* IP addresses */
   ip_addr_init(&BUCKET->L3->ip_src, AF_INET6, (u_char *)&ip6->saddr);
   ip_addr_init(&BUCKET->L3->ip_dst, AF_INET6, (u_char *)&ip6->daddr);
   
   /* other relevant infos */
   BUCKET->L3->proto = htons(LL_TYPE_IP6);
   BUCKET->L3->ttl = ip6->hop_limit;

   /* XXX had to implement passive fingerprint for IPv6 */
   
   switch (ip6->next_hdr) {
      case 0:
	 USER_MSG(" --> option  Hop-By-Hop");
	 opt = 0;
	 break;
      case 43:
	 USER_MSG(" --> option  Routing");
	 opt = 0;
	 break;
      case 44:
	 USER_MSG(" --> option  Fragment");
	 opt = 0;
	 break;
      case 60:
	 USER_MSG(" --> option  Destination");
	 opt = 0;
	 break;
      case 59:
	 USER_MSG(" --> option  No-Next-Header");
	 opt = 0;
	 break;
      default:
	 opt = -1;
	 break;
   }
      
   /* if (opt == 0)
      return get_decoder(OPT6_LAYER, ip6->next_hdr);
   else */
      return get_decoder(PROTO_LAYER, ip6->next_hdr);
}

/* EOF */

// vim:ts=3:expandtab

