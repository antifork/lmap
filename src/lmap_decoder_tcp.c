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
#include <lmap_fingerprint.h>


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

/* tcp options */
#define TCPOPT_EOL              0
#define TCPOPT_NOP              1
#define TCPOPT_MAXSEG           2
#define TCPOPT_WSCALE           3
#define TCPOPT_SACKOK           4
#define TCPOPT_TIMESTAMP        8


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
   BUCKET->L4->proto = LN_TYPE_TCP;

   /* 
    * complete the passive fingerprint
    * we are intereste only in SYN or SYN+ACK packets 
    * else we can destroy the fingerprint
    */

   if (tcp->flags & TH_SYN) {
   
      u_char *opt_start, *opt_end;

      opt_start = (u_char *)(tcp + 1);
      opt_end = (u_char *)((int)tcp + tcp->off * 4);
      
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_WINDOW, ntohs(tcp->win));
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_TCPFLAG, (tcp->flags & TH_ACK) ? 1 : 0);
      /* this should be added to the len of ip header */
      fingerprint_push(BUCKET->L4->fingerprint, FINGER_LT, tcp->off * 4);
   
      while (opt_start < opt_end) {
         switch (*opt_start) {
            case TCPOPT_EOL: 
               /* end option EXIT */
               opt_start = opt_end;
               break;
            case TCPOPT_NOP:
               fingerprint_push(BUCKET->L4->fingerprint, FINGER_NOP, 1);
               opt_start++;
               break;
            case TCPOPT_SACKOK:
               fingerprint_push(BUCKET->L4->fingerprint, FINGER_SACK, 1);
               opt_start += 2;
               break;
            case TCPOPT_MAXSEG:
               opt_start += 2;
               fingerprint_push(BUCKET->L4->fingerprint, FINGER_MSS, ntohs(ptohs(opt_start)));
               opt_start += 2;
               break;
            case TCPOPT_WSCALE:
               opt_start += 2;
               fingerprint_push(BUCKET->L4->fingerprint, FINGER_WS, *opt_start);
               opt_start++;
               break;
            case TCPOPT_TIMESTAMP:
               fingerprint_push(BUCKET->L4->fingerprint, FINGER_TIMESTAMP, 1);
               opt_start++;
               opt_start += (*opt_start - 1);
               break;
            default:
               opt_start++;
               opt_start += (*opt_start - 1);
               break;
         }
      }
      
   } else {
      fingerprint_destroy(&BUCKET->L4->fingerprint);
   }

   /* return the next decoder */
   return get_decoder(APP_LAYER, ntohs(tcp->dport));
}

/* EOF */

// vim:ts=3:expandtab

