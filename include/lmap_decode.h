
#ifndef LMAP_DECODE_H
#define LMAP_DECODE_H

#include <pcap.h>
#include <netinet/in.h>

#include <lmap_db.h>

/* XXX - remove me ! (USER_MSG in decoders) */
#include <lmap_ui.h>

#define FUNC_DECODER(func) void * func(u_char *buf, u_int16 buflen, int *len, struct bucket *b)
#define FUNC_DECODER_PTR(func) void * (*func)(u_char *buf, u_int16 buflen, int *len, struct bucket *b)

#define DECODE_DATALEN   buflen
#define DECODE_DATA      buf
#define DECODED_LEN      *len
#define BUCKET           b


extern void lmap_decode(u_char *u, const struct pcap_pkthdr *pkthdr, const u_char *pkt);
extern int set_L2_decoder(int dlt);
void add_decoder(int level, int type, FUNC_DECODER_PTR(decoder));
void * get_decoder(int level, int type);

/* layer canonical name */

enum {
   IFACE_LAYER  = 1,
   LINK_LAYER   = 2,
   NET_LAYER    = 3,
   PROTO_LAYER  = 4,
   APP_LAYER    = 5,
   OPT6_LAYER   = 6,  /* IPv6 option layer */
};

/* interface layer types */

enum {
   LI_TYPE_ETH  =    DLT_EN10MB,
   LI_TYPE_WIFI =    DLT_IEEE802_11,
};
   
/* link layer types */

enum {
   LL_TYPE_IP   = 0x0800,
   LL_TYPE_IP6  = 0x86DD,
   LL_TYPE_ARP  = 0x0806,
};

/* network layer types */

enum {
   LN_TYPE_ICMP  = 0x01,
   LN_TYPE_ICMP6 = 0x3a,
   LN_TYPE_TCP   = 0x06,
   LN_TYPE_UDP   = 0x11,
};

/* proto layer types */

enum {
   LP_TYPE_DHCPD = 0x0043,     /* 67.udp  server */
   LP_TYPE_DHCPC = 0x0044,     /* 68.udp  client */
};

/* IPv6 options types */
/* NOTE: they may (but should not) conflict with network layer types!   */
/*       double check new definitions of either types.                  */

enum {
   LO6_TYPE_HBH = 0,   /* Hop-By-Hop */
   LO6_TYPE_RT  = 43,  /* Routing */
   LO6_TYPE_FR  = 44,  /* Fragment */
   LO6_TYPE_DST = 60,  /* Destination */
   LO6_TYPE_NO  = 59,  /* No Next Header */
};

#endif

/* EOF */

// vim:ts=3:expandtab

