#ifndef LMAP_INET6_H
#define LMAP_INET6_H

#include <netinet/in.h>

enum {
   IP6_ADDR_LEN            = 16,
   IP6_ASCII_MAX_ADDR_LEN  = 45,
   IP6_DETAILS_SIZE        = 17,
   NS_IN6ADDRSZ            = 16,
   NS_INT16SZ              = 2,
};

#define SI6A    struct in6_addr

extern const char *inet_ntop6(const char *src, char *dst, size_t size);
extern const char *inet_details6(const char *src, char *dst, size_t size);

#endif

/* EOF */

// vim:ts=3:expandtab

