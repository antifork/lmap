#ifndef LMAP_INET_H
#define LMAP_INET_H

#include <netinet/in.h>

#define SI6A               struct in6_addr

#define LINUX_ROUTE_FILE   "/proc/net/route"
#define RESOLV_FILE        "/etc/resolv.conf"

enum {
   NS_IN6ADDRSZ            = 16,
   NS_INT16SZ              = 2,

   IP_ADDR_LEN             = 4,
   IP6_ADDR_LEN            = 16,
   MAX_ADDR_LEN            = IP6_ADDR_LEN,
   
   ETH_ASCII_ADDR_LEN      = sizeof("ff:ff:ff:ff:ff:ff"), 
   IP_ASCII_ADDR_LEN       = sizeof("255.255.255.255")+1, 
   IP6_ASCII_ADDR_LEN      = sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")+1,
   
   IP6_DETAILS_SIZE        = sizeof("multic linkloc")+1,
};
   
struct ip_addr {
   int type;
   size_t addr_size;
   char addr[MAX_ADDR_LEN];
};

extern int ip_addr_init(struct ip_addr *sa, int type, char *addr);
extern const char *ip_addr_ntoa(struct ip_addr *sa, char *dst);
extern const char *ip_addr_details(struct ip_addr *sa, char *dst, size_t size);

#endif

/* EOF */

// vim:ts=3:expandtab

