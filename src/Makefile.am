
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
               lmap_threads.c \
               lmap_data.c \
               lmap_hash.c \
               lmap_ui.c \
               lmap_ncurses.c \
               lmap_ncurses_info.c \
               lmap_ncurses_menu.c \
               lmap_decode.c \
               lmap_decoder_raw.c \
               lmap_decoder_eth.c \
               lmap_decoder_wifi.c \
               lmap_decoder_ip.c \
               lmap_decoder_icmp.c \
               lmap_decoder_arp.c \
               lmap_decoder_udp.c \
               lmap_decoder_tcp.c 


lmap_CFLAGS = @LMAP_CFLAGS@

lmap_LDADD = @LIBOBJS@ @LMAP_LIBS@ 

CLEANFILES = *.log *.out core.* missing/*.o *~

## MISSING FUNCTIONS

missing/strlcat.o:
	$(COMPILE) -c missing/strlcat.c -o missing/strlcat.o

missing/strlcpy.o:
	$(COMPILE) -c missing/strlcpy.c -o missing/strlcpy.o
	
missing/strsep.o:
	$(COMPILE) -c missing/strsep.c -o missing/strsep.o
	
missing/memmem.o:
	$(COMPILE) -c missing/memmem.c -o missing/memmem.o
	
missing/memcmp.o:
	$(COMPILE) -c missing/memcmp.c -o missing/memcmp.o
	
missing/getopt.o:
	$(COMPILE) -c missing/getopt.c -o missing/getopt.o
	
missing/getopt_long.o:
	$(COMPILE) -c missing/getopt_long.c -o missing/getopt_long.o
	
# vim:ts=4:noexpandtab
