
include $(top_srcdir)/Makefile.am.common

sbin_PROGRAMS = lmap

lmap_SOURCES = lmap_main.c \
               lmap_debug.c \
               lmap_error.c \
               lmap_signal.c \
               lmap_globals.c \
               lmap_parser.c \
               lmap_capture.c \
               lmap_inet.c \
               lmap_decode.c \
               lmap_decoder_eth.c \
               lmap_decoder_wifi.c \
               lmap_decoder_ip.c \
               lmap_decoder_icmp.c \
               lmap_decoder_arp.c \
               lmap_decoder_udp.c \
               lmap_decoder_tcp.c \
               missing/getopt.c \
               missing/getopt_long.c \
               missing/strlcat.c \
               missing/strlcpy.c

lmap_CFLAGS = @LMAP_CFLAGS@

lmap_LDADD = @LMAP_LIBS@

CLEANFILES = *.log *.out core.*

