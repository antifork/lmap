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

#ifdef OS_SOLARIS
#include <lmap_if_solaris.h>
#endif

struct in6_ifreq {
   struct in6_addr ifr6_addr;
   u_int32 if6_prefixlen;
   unsigned int ifr6_ifindex;
};

/* protos... */

int eth_addr_ntoa(const u_int8 *ll_addr, u_char *ascii);

int get_iface_idx(const char *iface);

int get_iface_ip(const char *iface, struct ip_addr *ipa, const int type);
int set_iface_ip(const char *iface, const struct ip_addr *ipa, const int type);

int get_iface_ll(const char *iface, char *ll_addr);
int set_iface_ll(const char *iface, const char *ll_addr);

int get_iface_mask(const char *iface, u_int32 *netmask);
int set_iface_mask(const char *iface, const u_int32 *netmask);

int get_default_gw(u_int32 *gw_addr);
int get_default_dns(u_int32 *dns_addr);


/*******************************************/

/* convert a link layer address to a printable dot notation */

int eth_addr_ntoa(const u_int8 *ll_addr, u_char *ascii)
{
   sprintf(ascii, "%02X:%02X:%02X:%02X:%02X:%02X", ll_addr[0], ll_addr[1],
                   ll_addr[2], ll_addr[3], ll_addr[4], ll_addr[5]);
   
   return ESUCCESS;
}

/* return index of iface */

int get_iface_idx(const char *iface)
{
   int sock;
   struct ifreq ifr;

   sock = socket(AF_INET, SOCK_DGRAM, 0);
   
   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
      ERROR_MSG("ioctl(SIOCGIFINDEX)");

   return ifr.ifr_ifindex;
}

/* return the IP address for the specified iface */
/* XXX convert to ip_addr and support IPv6 */

int get_iface_ip(const char *iface, struct ip_addr *ipa, const int type)
{
   int sock;
   struct ifreq ifr;
   
#ifdef OS_LINUX

#endif
   
   if (ipa == NULL)
      return -EINVALID;

   switch (type) {
   case AF_INET:
      sock = socket(AF_INET, SOCK_DGRAM, 0);

      memset(&ifr, 0, sizeof(ifr));
      strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

      if ( ioctl(sock, SIOCGIFADDR, &ifr) < 0 ) {
         close(sock);
         return -ENOADDRESS;
      }
   
      ip_addr_init(ipa, AF_INET, ifr.ifr_addr.sa_data + 2);
        
      close(sock);
      break;
   case AF_INET6:
#ifdef OS_LINUX
    
#endif
      break;
   default:
      ERROR_MSG("invalid address type");
      break;
   }

   return ESUCCESS;
}

/* set the IP address for the specified iface */
/* XXX convert to ip_addr and support IPv6 */

int set_iface_ip(const char *iface, const struct ip_addr *ipa, const int type)
{
   int sock;
   struct ifreq ifr;

   if (ipa == NULL)
      return -EINVALID;

   switch (type) {
   case AF_INET:
         
      sock = socket(AF_INET, SOCK_DGRAM, 0);

      memset(&ifr, 0, sizeof(ifr));
      strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));
      ifr.ifr_addr.sa_family = AF_INET;
      strncpy(ifr.ifr_addr.sa_data + 2, (char *)&ipa->addr, IP_ADDR_LEN);

      if ( ioctl(sock, SIOCSIFADDR, &ifr) < 0 ) {
         close(sock);
         return -ENOADDRESS;
      }

      close(sock);
      break;
   case AF_INET6:
      return -ENOADDRESS;
   };

   return ESUCCESS;
}


/* return the NETMASK for the specified iface */

int get_iface_mask(const char *iface, u_int32 *netmask)
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


/* set the NETMASK for the specified iface */

int set_iface_mask(const char *iface, const u_int32 *netmask)
{
   int sock;
   struct ifreq ifr;

   if (netmask == NULL)
      return -EINVALID;

   sock = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   ifr.ifr_addr.sa_family = AF_INET;
   memcpy(ifr.ifr_addr.sa_data + 2, netmask, IP_ADDR_LEN);

   if ( ioctl(sock, SIOCSIFNETMASK, &ifr) < 0 ) {
      close(sock);
      return -ENOADDRESS;
   }

   return ESUCCESS;
}
   
/* return the LINK LAYER address for the specified iface */

int get_iface_ll(const char *iface, char *ll_addr)
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
      ERROR_MSG("ioctl(SIOCGIFHWADDR)");
   
   memcpy(ll_addr, ifr.ifr_addr.sa_data, ETH_ADDR_LEN);
   
   close(sock);
#elif defined OS_BSD
   int mib[6];
   size_t len;
   char *buf, *next, *end;
   struct if_msghdr *ifm;
   struct sockaddr_dl *sdl;

   mib[0] = CTL_NET;
   mib[1] = AF_ROUTE;
   mib[2] = 0;
   mib[3] = AF_LINK;
   mib[4] = NET_RT_IFLIST;
   mib[5] = 0;

   if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
      ERROR_MSG("sysctl()");

   if ((buf = (char *)malloc(len)) == NULL )
      ERROR_MSG("malloc()");

   if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
      ERROR_MSG("sysctl()");

   end = buf + len;

   for (next = buf ; next < end ; next += ifm->ifm_msglen)
   {
      ifm = (struct if_msghdr *)next;
      if (ifm->ifm_type == RTM_IFINFO)
      {
         sdl = (struct sockaddr_dl *)(ifm + 1);
         if (strncmp(&sdl->sdl_data[0], iface, sdl->sdl_nlen) == 0)
         {
            memcpy(ll_addr, LLADDR(sdl), ETHER_ADDR_LEN);
            break;
         }
      }
   }
      
   SAFE_FREE(buf);
#elif defined OS_SOLARIS
   int sock, dlpi;
   char buf[2048];
   union DL_primitives *dlp;

   dlpi = if_open_raw(iface);

   dlp = (union DL_primitives*) buf;

   dlp->physaddr_req.dl_primitive = DL_PHYS_ADDR_REQ;
   dlp->physaddr_req.dl_addr_type = DL_CURR_PHYS_ADDR;

   if (send_request(dlpi, (char *)dlp, DL_PHYS_ADDR_REQ_SIZE, "physaddr", ebuf) < 0)
      Error_msg("send_request(DL_PHYS_ADDR_REQ_SIZE) | %s \n", ebuf);

   if (recv_ack(dlpi, DL_PHYS_ADDR_ACK_SIZE, "physaddr", (char *)dlp, ebuf) < 0)
      Error_msg("recv_ack(DL_PHYS_ADDR_ACK_SIZE) | %s \n", ebuf);

   memcpy( MyMAC,(struct ether_addr *) ((char *) dlp + dlp->physaddr_ack.dl_addr_offset), ETHER_ADDR_LEN);

   /* raw sock fd is closed on exit */
#else
   #error get_iface_ll not implemented under this os
#endif
   
   return ESUCCESS;
}


/* set the LINK LAYER address for the specified iface */

int set_iface_ll(const char *iface, const char *ll_addr)
{
#ifdef OS_LINUX
   int sock;
   struct ifreq ifr;

   if (ll_addr == NULL)
      return -EINVALID;

   sock = socket(AF_INET, SOCK_DGRAM, 0);

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));

   memcpy(ifr.ifr_addr.sa_data, ll_addr, ETH_ADDR_LEN);

   if ( ioctl(sock, SIOCSIFHWADDR, &ifr) < 0 )
      ERROR_MSG("ioctl(SIOCSIFHWADDR");

#elif defined OS_BSD
   #error set_iface_ll is WIP under BSD
#elif defined OS_SOLARIS
   #error set_iface_ll is WIP under SOLARIS
#else
   #error get_iface_ll not implemented under this os
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

