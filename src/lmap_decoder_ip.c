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
#include <lmap_fingerprint.h>

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
#define IP_DF 0x4000
   u_int8   ttl;
   u_int8   protocol;
   u_int16  check;
   u_int32  saddr;
   u_int32  daddr;
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

   update_stat("IP", 1);
   
   ip = (struct ip_header *)DECODE_DATA;
  
   DECODED_LEN = ip->ihl * 4;

   /* IP addresses */
   ip_addr_init(&BUCKET->L3->ip_src, AF_INET, (char *)&ip->saddr);
   ip_addr_init(&BUCKET->L3->ip_dst, AF_INET, (char *)&ip->daddr);
   
   /* other relevant infos */
   BUCKET->L3->proto = htons(LL_TYPE_IP);
   BUCKET->L3->ttl = ip->ttl;

   /* if there is a TCP packet, try to passive fingerprint it */
   if (ip->protocol == LN_TYPE_TCP) {
      /* initialize passive fingerprint */
      BUCKET->L4->fingerprint = fingerprint_alloc();
  
      /* collect ifos for passive fingerprint */
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_TTL, ip->ttl);
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_DF, ntohs(ip->frag_off) & IP_DF);
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_LT, ip->ihl * 4);
   }
   
#if 0
   if (ip->ihl * 4 != sizeof(struct ip_header))
      USER_MSG(" --> IP OPTIONS PRESENT (%d byte)\n", (ip->ihl * 4) - sizeof(struct ip_header));
#endif
   
   return get_decoder(PROTO_LAYER, ip->protocol);
}

/* EOF */

// vim:ts=3:expandtab

