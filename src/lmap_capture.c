/*
    lmap -- iface and capture functions

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header$
*/

#include <lmap.h>
#include <lmap_decode.h>
#include <lmap_threads.h>
#include <lmap_ui.h>

#include <sys/socket.h>

#include <pcap.h>

#define PCAP_PROMISC 1
#define PCAP_TIMEOUT 0

void capture_init(void);
void capture_close(void);
LMAP_THREAD_FUNC(capture);

/*******************************************/

/*
 * set up the pcap to capture from the specified interface
 * set up even the first dissector by looking at DLT_*
 */

void capture_init(void)
{
   pcap_t *pd;
   pcap_dumper_t *pdump;
   int dlt;
   char pcap_errbuf[PCAP_ERRBUF_SIZE];
   
   /*
    * if the user didn't specified the interface,
    * we have to found one...
    */
   
   if (GBL_OPTIONS->iface == NULL) {
      char *ifa = pcap_lookupdev(pcap_errbuf);
      ON_ERROR(ifa, NULL, "No suitable interface found...");
      
      GBL_OPTIONS->iface = strdup(ifa);
   }
   
   DEBUG_MSG("capture_init %s", GBL_OPTIONS->iface);
              
   USER_MSG("Listening on on %s...\n\n", GBL_OPTIONS->iface);
   
   /*
    * set the snaplen to maximum
    */
   
   GBL_PCAP->snaplen = 9999;
   
   /* 
    * open the interface from GBL_OPTIONS (user specified)
    */

   if (GBL_OPTIONS->read)
      pd = pcap_open_offline(GBL_OPTIONS->dumpfile, pcap_errbuf);
   else
      pd = pcap_open_live(GBL_OPTIONS->iface, GBL_PCAP->snaplen, PCAP_PROMISC, 
                   PCAP_TIMEOUT, pcap_errbuf);
   
   ON_ERROR(pd, NULL, "%s", pcap_errbuf);


   if (GBL_OPTIONS->dump) {
      pdump = pcap_dump_open(pd, GBL_OPTIONS->dumpfile);
      GBL_PCAP->dump = pdump;               
   }
   
   /*
    * set the right decoder for L2
    */
   
   dlt = pcap_datalink(pd);
   
   if (set_L2_decoder(dlt) != ESUCCESS) {
      if (GBL_OPTIONS->read)
         FATAL_MSG("Dump file not supported (DLT = %d)", dlt);
      else
         FATAL_MSG("Inteface %s not supported (DLT = %d)", GBL_OPTIONS->iface, dlt);
   }
   
   
   GBL_PCAP->pcap = pd;               
 
   atexit(capture_close);
   
}


void capture_close(void)
{
   pcap_close(GBL_PCAP->pcap);
   if (GBL_OPTIONS->dump)
      pcap_dump_close(GBL_PCAP->dump);
   
   DEBUG_MSG("capture_closed");
}

/*
 * start capturing packets
 */

LMAP_THREAD_FUNC(capture)
{
   DEBUG_MSG("neverending loop (capture)");
  
   lmap_thread_init();
   
   /* 
    * infinite loop 
    * dispatch packets to sarp_get
    */
        
   pcap_loop(GBL_PCAP->pcap, -1, lmap_decode, NULL);

   return NULL;
}


/* EOF */

// vim:ts=3:expandtab

