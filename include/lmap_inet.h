
#ifndef LMAP_INET_H
#define LMAP_INET_H

#define ETH_ADDR_LEN 6   /* ethernet address len */
#define IP_ADDR_LEN 4   /* ip address len */

#define ETH_ASCII_ADDR_LEN 17   /* ethernet address len in ascii format*/
#define IP_ASCII_ADDR_LEN 15   /* ip address len in asci format */

#define LINUX_ROUTE_FILE "/proc/net/route"

/*  XXX remove this.... they are not thread safe !! */
extern char * ha_ntoa(const u_char *ll_addr);
extern char * ha_aton(const u_char *ll_addr);
extern char * pa_ntoa(const u_char *ip_addr);
extern u_int32 pa_aton(const u_char *ip_addr);
/***************************************************/

int pa_ntoa_r(u_int32 ip_addr, u_char *ascii);
int ha_ntoa_r(u_int8 *ll_addr, u_char *ascii);

extern int get_iface_ip(char *iface, u_int32 *ip_addr);
extern int get_iface_ll(char *iface, char *ll_addr);
extern int get_iface_mask(char *iface, u_int32 *netmask);
extern int get_default_gw(u_int32 *gw_addr);

#endif

/* EOF */

// vim:ts=3:expandtab

