#ifndef LMAP_IF_H
#define LMAP_IF_H
 
#include <lmap_inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define LINUX_ROUTE_FILE "/proc/net/route"
#define RESOLV_FILE "/etc/resolv.conf"

enum {
   ETH_ADDR_LEN = 6,
};

extern int eth_addr_ntoa(const u_int8 *ll_addr, u_char *ascii);

extern int get_iface_idx(const char *iface);

extern int get_iface_ip(const char *iface, struct ip_addr *ipa, const int type);
extern int set_iface_ip(const char *iface, const struct ip_addr *ipa, const int type);

extern int get_iface_ll(const char *iface, char *ll_addr);
extern int set_iface_ll(const char *iface, const char *ll_addr);

extern int get_iface_mask(const char *iface, u_int32 *netmask);
extern int set_iface_mask(const char *iface, const u_int32 *netmask);

extern int get_default_gw(u_int32 *gw_addr);
extern int get_default_dns(u_int32 *gw_addr);

#endif

/* EOF */

// vim:ts=3:expandtab

