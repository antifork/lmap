
#ifndef LMAP_INET_H
#define LMAP_INET_H

#define ETH_ADDR_LEN 6   /* ethernet address len */
#define IP_ADDR_LEN 4   /* ip address len */

#define ETH_ASCII_ADDR_LEN 17   /* ethernet address len in ascii format*/
#define IP_ASCII_ADDR_LEN 15   /* ip address len in asci format */

extern char * ha_ntoa(const u_char *ll_addr);
extern char * ha_aton(const u_char *ll_addr);
extern char * pa_ntoa(const u_char *ip_addr);
extern u_int32 pa_aton(const u_char *ip_addr);


#endif

/* EOF */

// vim:ts=3:expandtab

