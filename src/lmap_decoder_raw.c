/*
    lmap -- RAW decoder module

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
#include <lmap_if.h>


/* globals */

/* protos */

FUNC_DECODER(decode_raw);
void raw_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init raw_init(void)
{
   add_decoder(LINK_LAYER, LI_TYPE_RAW, decode_raw);
}


FUNC_DECODER(decode_raw)
{
   
   DECODED_LEN = DECODE_DATALEN;
   
   
   USER_MSG("RAW : 0x%04x bytes\n%s", 
                   DECODE_DATALEN,
                   hex_format(DECODE_DATA, DECODED_LEN));
   
   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

