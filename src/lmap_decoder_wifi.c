/*
    lmap -- 802.11b decoder module

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

struct wifi_header {
   u_int16  type;
   u_int16  duration;
   u_int8   dest[ETH_ADDR_LEN];
   u_int8   source[ETH_ADDR_LEN];
   u_int8   bssid[ETH_ADDR_LEN];
   u_int16  seq;
};

struct wifi_ll_header {
   u_int8   dsap;
   u_int8   ssap;
   u_int8   control;
   u_int8   organization[3];
   u_int16  type;
};

/* protos */

FUNC_DECODER(decode_wifi);
void wifi_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init wifi_init(void)
{
   add_decoder(LINK_LAYER, LI_TYPE_WIFI, decode_wifi);
}


FUNC_DECODER(decode_wifi)
{
   struct wifi_header *wifi;
   struct wifi_ll_header *wifi_ll;
   void *next_decoder = NULL;

   update_stat("TOTAL", 1);
   
   DECODED_LEN = sizeof(struct wifi_header);
      
   wifi = (struct wifi_header *)DECODE_DATA;

   /* we are only interested in "data" (0x0802) type */
   
   if (ntohs(wifi->type) == 0x0802) {
      wifi_ll = (struct wifi_ll_header *)(wifi + 1);
      DECODED_LEN += sizeof(struct wifi_ll_header);
      next_decoder = get_decoder(NET_LAYER, ntohs(wifi_ll->type));
   }

   if (ntohs(wifi->type) == 0x0800) {
      DECODED_LEN = DECODE_DATALEN;
      next_decoder = NULL;
      USER_MSG("WIFI: BACON (or unsupported message)");
   }
   
   USER_MSG("WIFI : 0x%04x bytes\n%s\n", 
                   DECODE_DATALEN, 
                   hex_format(DECODE_DATA, DECODED_LEN));

   return next_decoder;
}

/* EOF */

// vim:ts=3:expandtab

