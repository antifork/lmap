/*
    lmap -- DHCP decoder module

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


/* globals */


/* protos */

FUNC_DECODER(decode_dhcpd);
FUNC_DECODER(decode_dhcpc);
void dhcp_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init dhcp_init(void)
{
   add_decoder(APP_LAYER, LP_TYPE_DHCPD, decode_dhcpd);
   add_decoder(APP_LAYER, LP_TYPE_DHCPC, decode_dhcpc);
   register_stat("DHCP");
}


/* server messages */

FUNC_DECODER(decode_dhcpd)
{
   (void)DECODE_DATA;

   DECODED_LEN = 0;

   USER_MSG("DHCP -- server");
   DEBUG_MSG("DHCP -- server");

   return NULL;
}


/* client messages */

FUNC_DECODER(decode_dhcpc)
{
   (void)DECODE_DATA;

   DECODED_LEN = 0;

   USER_MSG("DHCP -- client");
   DEBUG_MSG("DHCP -- client");
   
   return NULL;
}


/* EOF */

// vim:ts=3:expandtab

