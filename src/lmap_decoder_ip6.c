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
#include <lmap_inet6.h>

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

   u_int8   saddr[16];
   u_int8   daddr[16];
   
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
   register_stat("IPv6");
}


FUNC_DECODER(decode_ip6)
{
   struct ip6_header *ip6;
   int opt; /* -1 means no options defined, if 0 an option is present */
   char paddr[IP6_ASCII_ADDR_LEN];
   char details[IP6_DETAILS_SIZE];
   
   update_stat("IPv6", 1);
   
   ip6 = (struct ip6_header *)DECODE_DATA;
  
   
   DECODED_LEN = 0 /* XXX calculate this */ ;

   USER_MSG("IPv6 : 0x%04x bytes\n%s", 
                   DECODE_DATALEN, 
                   hex_format(DECODE_DATA, DECODED_LEN));
   

   inet_ntop6((char *)&ip6->saddr, paddr, sizeof (paddr)); 
   inet_details6((char *)&ip6->saddr, details, sizeof(details));
   USER_MSG(" --> source  %s %s", paddr, details);

   inet_ntop6((char *)&ip6->daddr, paddr, sizeof (paddr));
   inet_details6((char *)&ip6->saddr, details, sizeof(details));
   USER_MSG(" --> dest    %s %s", paddr, details);
   
   USER_MSG(" --> proto   0x%x ", ip6->next_hdr);

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
      
   /*
   if (opt == 0)
      return get_decoder(OPT6_LAYER, ip6->next_hdr);
   else 
   */
   return get_decoder(PROTO_LAYER, ip6->next_hdr);
}

/* EOF */

// vim:ts=3:expandtab

