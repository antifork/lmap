#ifndef LMAP_DB_H
#define LMAP_DB_H

#include <lmap_inet.h>

/* bucket content */

struct L2_info {
   u_int8 mac_src[6];
   u_int8 mac_dst[6];
   char   *ESSID;
};

struct L3_info {
   struct ip_addr  ip_src;
   struct ip_addr  ip_dst;
   u_int16  proto;
   u_int8   ttl;
};

struct L4_info {
   u_int16 port_src;
   u_int16 port_dst;
   u_int16 proto;
   char    *fingerprint;
};
 
struct L5_info {
   int type;
   union {
      struct dhcp {
         struct ip_addr  ip;
         struct ip_addr  gw;
         struct ip_addr  dns;
      };
   } un;
};

/* 
 * database core elements 
 */

struct L5_entry {
   u_int64 id;
   struct L5_info  *inf;
   struct L5_entry *next;
};

struct L4_entry {
   u_int64 id;
   struct L4_info  *inf;
   struct L4_entry *next;
   struct L5_entry *sap;
};

struct L3_entry {
   u_int64 id;
   struct L3_info  *inf;
   struct L3_entry *next;
   struct L4_entry *sap;
};

struct L2_entry {
   u_int64 id;
   struct L2_info  *inf;
   struct L2_entry *next;
   struct L3_entry *sap;
};


/* 
 * the struct bucket is used by database in order to exchange messages
 * between applications and the engine 
 */
 
struct bucket {
   struct L2_info *L2;
   struct L3_info *L3;
   struct L4_info *L4;
   struct L5_info *L5;
};
   
 
/* extern prototypes */

extern struct bucket * db_bucket_alloc(void);
extern void db_bucket_free(struct bucket *);
extern void db_bucket_send(struct bucket *);


#endif

/* EOF */

// vim:ts=3:expandtab

