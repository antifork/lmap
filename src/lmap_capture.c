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

*/

#include <lmap.h>
#include <lmap_decode.h>

#include <sys/socket.h>

#include <pcap.h>

#define PCAP_PROMISC 1
#define PCAP_TIMEOUT 0

void capture_init(void);
void capture_close(void);
void capture(void);
char * default_iface(void);

/*******************************************/

/*
 * set up the pcap to capture from the specified interface
 * set up even the first dissector by looking at DLT_*
 */

void capture_init(void)
{
   pcap_t *pd;
   int dlt;
   char pcap_errbuf[PCAP_ERRBUF_SIZE];
   
   DEBUG_MSG("capture_init");

   if (GBL_OPTIONS->iface == NULL)
      GBL_OPTIONS->iface = default_iface(); 
   
   /*
    * set the snaplen to maximum
    */
   
   GBL_PCAP->snaplen = 9999;
   
   /* 
    * open the interface from GBL_OPTIONS (user specified)
    */
   
   pd = pcap_open_live(GBL_OPTIONS->iface, GBL_PCAP->snaplen, PCAP_PROMISC, 
                   PCAP_TIMEOUT, pcap_errbuf);
   ON_ERROR(pd, NULL, "%s", pcap_errbuf);

   /*
    * set the right decoder for L2
    */
   
   dlt = pcap_datalink(pd);
   
   if (set_L2_decoder(dlt) != ESUCCESS)
      FATAL_MSG("Inteface %s not supported (DLT = %d)", GBL_OPTIONS->iface, dlt);
   
   
   GBL_PCAP->pcap = pd;               
 
   atexit(capture_close);
   
}


void capture_close(void)
{
   pcap_close(GBL_PCAP->pcap);
   
   DEBUG_MSG("capture_closed");
}

/*
 * return the name of the first iface 
 * UP and running
 */

char * default_iface(void)
{
   /*
    * XXX to be implemented....
    * search the first up and running interface
    */
        
   return "eth0";        
}

/*
 * start capturing packets
 */

void capture(void)
{
   DEBUG_MSG("neverending loop (capture)");
   
   /* 
    * infinite loop 
    * dispatch packets to sarp_get
    */
        
   pcap_loop(GBL_PCAP->pcap, -1, lmap_decode, NULL);
}


/* EOF */

// vim:ts=3:expandtab

