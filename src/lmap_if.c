/*
    lmap -- interface stuff

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
#include <lmap_if.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>

/* protos... */

int eth_addr_ntoa(u_int8 *ll_addr, u_char *ascii);

int get_iface_ip(char *iface, u_int32 *ip_addr);
int get_iface_ll(char *iface, char *ll_addr);
int get_iface_mask(char *iface, u_int32 *netmask);
int get_default_gw(u_int32 *gw_addr);
int get_default_dns(u_int32 *dns_addr);


/*******************************************/

/* convert a link layer address to a printable dot notation */

int eth_addr_ntoa(u_int8 *ll_addr, u_char *ascii)
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
#elif defined OS_BSD
   #error get_iface_ll is WIP
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

int get_default_dns(u_int32 *dns_addr)
{
   FILE *dns_fp = (FILE *)NULL;
   char line[1024];
   char *p;

   if( (dns_fp = fopen(RESOLV_FILE, "r")) == (FILE *)NULL ) {
      return(EINVALID);
   }

   while( fgets(line, sizeof(line) - 1, dns_fp) != (char *)NULL ) {
      if ( (strcmp( (p = strtok(line, " ")), "nameserver" ) == 0 ) ) 
      {  
         struct in_addr ina;
         
         p = strtok(NULL, "\n");
         inet_aton(p, &ina);
         memcpy(dns_addr, &ina, sizeof(u_int32));
         
         return ESUCCESS;
      }
   }

   fclose(dns_fp);
   return(ENOTFOUND);
}

/* EOF */

// vim:ts=3:expandtab

