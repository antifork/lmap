#ifndef LMAP_IF_H
#define LMAP_IF_H
 

#include <lmap_inet.h> /* XXX should be removed when done separating! */

#define LINUX_ROUTE_FILE "/proc/net/route"
#define RESOLV_FILE "/etc/resolv.conf"

enum {
   ETH_ADDR_LEN = 6,
};

extern int eth_addr_ntoa(u_int8 *ll_addr, u_char *ascii);

extern int get_iface_ip(char *iface, u_int32 *ip_addr);
extern int get_iface_ll(char *iface, char *ll_addr);
extern int get_iface_mask(char *iface, u_int32 *netmask);
extern int get_default_gw(u_int32 *gw_addr);
extern int get_default_dns(u_int32 *gw_addr);

#endif

/* EOF */

// vim:ts=3:expandtab

