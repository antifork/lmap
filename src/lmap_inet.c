/*
    lmap -- inet stuff

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
#include <lmap_inet.h>

#include <netinet/in.h>
#include <arpa/inet.h>


/* protos... */


char * ha_ntoa(const u_char *ll_addr);
char * ha_aton(const u_char *ll_addr);
char * pa_ntoa(const u_char *ip_addr);
u_int32 pa_aton(const u_char *ip_addr);


/*******************************************/


/*
 * convert a ip address to a printable dot notation
 */

char * pa_ntoa(const u_char *ip)
{
   return inet_ntoa(*(struct in_addr *)ip);
}

/*
 * convert a ip address in dot notation to interger
 */

u_int32 pa_aton(const u_char *ip)
{
   return inet_addr(ip);
}

/*
 * convert a link layer address to a printable colon separated format
 */

char * ha_ntoa(const u_char *ll_addr)
{
   static char printable[18];

   sprintf(printable, "%02X:%02X:%02X:%02X:%02X:%02X", ll_addr[0], ll_addr[1],
                   ll_addr[2], ll_addr[3], ll_addr[4], ll_addr[5]);
   
   return printable;
}

/*
 * convert a link layer address from ascii to network
 */

char * ha_aton(const u_char *ll_addr)
{
   static char network[ETH_ADDR_LEN];
   int m1,m2,m3,m4,m5,m6;

   if (sscanf(ll_addr, "%02X:%02X:%02X:%02X:%02X:%02X", &m1, &m2, &m3, 
                           &m4, &m5, &m6) != 6)
      return NULL;
   
   network[0] = (char) m1;
   network[1] = (char) m2;
   network[2] = (char) m3;
   network[3] = (char) m4;
   network[4] = (char) m5;
   network[5] = (char) m6;
   
   return network;
}


/* EOF */

// vim:ts=3:expandtab

