
#ifndef LMAP_DECODE_H
#define LMAP_DECODE_H

#include <pcap.h>
#include <netinet/in.h>

/* XXX - remove me ! */
#include <lmap_ui.h>

#define FUNC_DECODER(func) void * func(u_char *buf, u_int16 buflen, int *len)
#define FUNC_DECODER_PTR(func) void * (*func)(u_char *buf, u_int16 buflen, int *len)

#define DECODE_DATALEN   buflen
#define DECODE_DATA      buf
#define DECODED_LEN      *len


extern void lmap_decode(u_char *u, const struct pcap_pkthdr *pkthdr, const u_char *pkt);
extern int set_L2_decoder(int dlt);
void add_L2_decoder(int dlt, FUNC_DECODER_PTR(decoder));
void add_decoder(int level, int type, FUNC_DECODER_PTR(decoder));
void * get_decoder(int level, int type);

/* layer canonical name */

#define IFACE_LAYER  1
#define LINK_LAYER   2
#define NET_LAYER    3
#define PROTO_LAYER  4
#define APP_LAYER    5

/* interface layer types */

#define LI_TYPE_RAW     0
#define LI_TYPE_ETH     DLT_EN10MB
#define LI_TYPE_WIFI    DLT_IEEE802_11

/* link layer types */

#define LL_TYPE_IP      0x0800
#define LL_TYPE_ARP     0x0806

/* network layer types */

#define LN_TYPE_ICMP    0x01
#define LN_TYPE_TCP     0x06
#define LN_TYPE_UDP     0x11

/* proto layer types */

#define LP_TYPE_SSH     0x0012


#endif

/* EOF */

// vim:ts=3:expandtab

