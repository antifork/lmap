/*
    lmap -- ETH decoder module

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
#include <lmap_inet.h>


/* globals */

#define ETH_ALEN 6

struct eth_header
{
   u_int8   dha[ETH_ALEN];       /* destination eth addr */
   u_int8   sha[ETH_ALEN];       /* source ether addr */
   u_int16  proto;               /* packet type ID field */
};


/* protos */

FUNC_DECODER(decode_eth);
void eth_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init eth_init(void)
{
   add_decoder(LINK_LAYER, LI_TYPE_ETH, decode_eth);
}

extern FUNC_DECODER(decode_ip);

FUNC_DECODER(decode_eth)
{
   struct eth_header *eth;
   
   DECODED_LEN = sizeof(struct eth_header);
   
   eth = (struct eth_header *)DECODE_DATA;
   
   DEBUG_MSG("ETH : 0x%04x bytes\n%s", 
                   DECODE_DATALEN,
                   hex_format(DECODE_DATA, DECODED_LEN));
   
   DEBUG_MSG(" --> source  %s", ha_ntoa(eth->sha));
   DEBUG_MSG(" --> dest    %s", ha_ntoa(eth->dha));
   DEBUG_MSG(" --> type    0x%04x\n", ntohs(eth->proto));
   
   return get_decoder(NET_LAYER, ntohs(eth->proto));
}

/* EOF */

// vim:ts=3:expandtab

