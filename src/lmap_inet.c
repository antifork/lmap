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

    $Header$
*/

#include <lmap.h>
#include <lmap_inet.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>

/* protos... */


/* XXX remove these */
char * ha_ntoa(const u_char *ll_addr);
char * ha_aton(const u_char *ll_addr);
char * pa_ntoa(const u_char *ip_addr);
u_int32 pa_aton(const u_char *ip_addr);
/********************/

int pa_ntoa_r(u_int32 ip_addr, u_char *ascii);
int ha_ntoa_r(u_int8 *ll_addr, u_char *ascii);

int get_iface_ip(char *iface, u_int32 *ip_addr);
int get_iface_ll(char *iface, char *ll_addr);
int get_iface_mask(char *iface, u_int32 *netmask);
int get_default_gw(u_int32 *gw_addr);


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


/* convert a ip address to a printable dot notation */

int pa_ntoa_r(u_int32 ip_addr, u_char *ascii)
{
   u_char *p;

   p = (u_char *)&ip_addr;
   
   sprintf(ascii, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
        
   return ESUCCESS;
}

/* convert a link layer address to a printable dot notation */

int ha_ntoa_r(u_int8 *ll_addr, u_char *ascii)
{
   sprintf(ascii, "%02X:%02X:%02X:%02X:%02X:%02X", ll_addr[0], ll_addr[1],
                   ll_addr[2], ll_addr[3], ll_addr[4], ll_addr[5]);
   
   return ESUCCESS;
}


/* return the IP address for the specified iface */

int get_iface_ip(char *iface, u_int32 *ip_addr)
{
   int sock;
   struct ifreq ifr;

   if (ip_addr == NULL)
      return -EINVALID;

   sock = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   if ( ioctl(sock, SIOCGIFADDR, &ifr) < 0 ) {
      close(sock);
      return -ENOADDRESS;
   }
   
   memcpy((char *)ip_addr, ifr.ifr_addr.sa_data + 2, IP_ADDR_LEN);
        
   close(sock);
   
   return ESUCCESS;
}

/* return the NETMASK for the specified iface */

int get_iface_mask(char *iface, u_int32 *netmask)
{
   int sock;
   struct ifreq ifr;

   if (netmask == NULL)
      return -EINVALID;
   
   sock = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   if ( ioctl(sock, SIOCGIFNETMASK, &ifr) < 0 ) {
      close(sock);
      return -ENOADDRESS;
   }
   
   memcpy((char *)netmask, ifr.ifr_addr.sa_data + 2, IP_ADDR_LEN);
   
   close(sock);
   
   return ESUCCESS;
}

/* return the LINK LAYER address for the specified iface */

int get_iface_ll(char *iface, char *ll_addr)
{
#ifdef OS_LINUX
   int sock;
   struct ifreq ifr;

   if (ll_addr == NULL)
      return -EINVALID;
   
   sock = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   if ( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 )
      ERROR_MSG("ioctl(SIOCGIFNETMASK)");
   
   memcpy(ll_addr, ifr.ifr_addr.sa_data, ETH_ADDR_LEN);
   
   close(sock);
#else
   #error get_iface_ll implemented only under linux
#endif
   
   return ESUCCESS;
}

int get_default_gw(u_int32 *gw_addr)
{
#ifdef OS_LINUX
   FILE *rf_fp = (FILE *)NULL;
   char line[1024];
   u_int32 dest;
   u_int16 flags;

   if((rf_fp = fopen(LINUX_ROUTE_FILE, "r")) == (FILE *)NULL) {
      return(EINVALID);
   }

   fgets(line, sizeof(line) - 1, rf_fp);
   while(fgets(line, sizeof(line) - 1, rf_fp) != (char *)NULL) {
      if(sscanf(line + 8, "%x %x %hx", &dest, gw_addr, &flags) != 3) {
	 continue;
      }
      if((flags & RTF_GATEWAY) && (dest == INADDR_ANY)) {
         fclose(rf_fp);
         return(ESUCCESS);
      }
   }
   fclose(rf_fp);
   return(ENOTFOUND);

#else
   #error get_default_gw implemented only under linux
#endif
}


/* EOF */

// vim:ts=3:expandtab

