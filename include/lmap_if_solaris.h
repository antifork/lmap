#ifndef _LMAP_IF_SOLARIS_H
#define _LMAP_IF_SOLARIS_H

/* 
 * this header contains all the solaris socket primitives used in lmap_if.c
 * IT'S UNDERGOING REVISION AND ACTUALLY DOES NOT WORK!
 */

#include <ctype.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/sockio.h>

#include <sys/types.h>
#include <sys/time.h>
#ifdef HAVE_SYS_BUFMOD_H
#include <sys/bufmod.h>
#endif
#include <sys/dlpi.h>
#ifdef HAVE_SYS_DLPI_EXT_H
#include <sys/dlpi_ext.h>
#endif

#include <sys/stream.h>
#ifdef HAVE_SYS_BUFMOD_H
#include <sys/systeminfo.h>
#endif

#include <sys/stropts.h>
#include <inet/nd.h>
#include <inet/common.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <unistd.h>

/* XXX NOT TESTED! had to fix the whole solaris if management code! */

int if_open_raw(char *iface);
int recv_ack(int fd, int size, const char *what, char *bufp, char *ebuf);
int send_request(int fd, char *ptr, int len, char *what, char *ebuf);

int if_open_raw(char *iface)
{
   register char *cp;
   int fd;
   int ppa;
   register dl_info_ack_t *infop;
   struct timeval to;
#ifdef HAVE_SYS_BUFMOD_H
   u_int32 flag, ss;
   char *release;
   u_int osmajor, osminor, osmicro;
#endif
   u_long buf[MAXDLBUF];
   char dname[100];
#ifndef HAVE_DEV_DLPI
   char dname2[100];
#endif

   if (dlpi_in_use != 0) {
      DEBUG_MSG("Inet_OpenRawSock %s", iface);
      DEBUG_MSG("Inet_OpenRawSock \t dlpi_in_use = %d ", dlpi_in_use);
      return dlpi_in_use;
   }

#ifdef HAVE_DEV_DLPI
   /*
   ** Remove any "/dev/" on the front of the device.
   */
   cp = strrchr(iface, '/');
   if (cp == NULL)
      cp = iface;
   else
      cp++;
   strlcpy(dname, cp, sizeof(dname));

   DEBUG_MSG("Inet_OpenRawSock -- HAVE_DEV_DLPI [%s]", dname);

   /*
    * Split the name into a device type and a unit number.
    */

   cp = split_dname(dname, &ppa);
   if (cp == NULL)
      Error_msg("ec_inet_solaris:%d HAVE_DEV_DLPI: %s missing or bad unit number\n", __LINE__, iface);

	*cp = '\0';

   /*
    * Use "/dev/dlpi" as the device.
    */

   cp = "/dev/dlpi";
   if ((fd = open(cp, O_RDWR)) < 0)
      ERROR_MSG("open()");

   /*
    * Get a table of all PPAs for that device, and search that
    * table for the specified device type name and unit number.
    */
   ppa = get_dlpi_ppa(fd, dname, ppa, ebuf);
   if (ppa < 0)
      ERROR_MSG("get_dlpi_ppa()");

#else
   /*
   ** Determine device and ppa
   */
   cp = split_dname(iface, &ppa);
   if (cp == NULL)
      Error_msg("ec_inet_solaris:%d dlpi: %s missing or bad unit number\n", __LINE__, iface);

   if (*iface == '/')
      strlcpy(dname, iface, sizeof(dname));
   else
      snprintf(dname, sizeof(dname), "%s/%s", DLPI_DEV_PREFIX, iface);

   strlcpy(dname2, dname, sizeof(dname2));
   *(dname + strlen(dname) - strlen(cp)) = '\0';

   DEBUG_MSG("Inet_OpenRawSock -- opening [%s]", dname);

   /* Try device without unit number */
   if ((fd = open(dname, O_RDWR)) < 0)
   {
      if (errno != ENOENT)
         ERROR_MSG("open()");

      DEBUG_MSG("Inet_OpenRawSock -- open(%s) failed... trying [%s]", dname, dname2);

      /* Try again with unit number */
      if ((fd = open(dname2, O_RDWR)) < 0)
         ERROR_MSG("open()");

      /* XXX Assume unit zero */
      ppa = 0;
   }
#endif

   /*
   ** Attach if "style 2" provider
   */
   if (dlinforeq(fd, ebuf) < 0 || dlinfoack(fd, (char *)buf, ebuf) < 0)
      ERROR_MSG("dlinforeq() || dlinfoack()");


   infop = &((union DL_primitives *)buf)->info_ack;
   if (infop->dl_provider_style == DL_STYLE2 && (dlattachreq(fd, ppa, ebuf) < 0 ||
       dlokack(fd, "attach", (char *)buf, ebuf) < 0))
      ERROR_MSG("dlattachreq()");

   /*
   ** Bind
   */
   if (dlbindreq(fd, 0, ebuf) < 0 || dlbindack(fd, (char *)buf, ebuf) < 0)
      ERROR_MSG("dlbindreq()");


   if (dlpromisconreq(fd, DL_PROMISC_PHYS, ebuf) < 0 || dlokack(fd, "promisc_phys", (char *)buf, ebuf) < 0)
      ERROR_MSG("dlpromisconreq(DL_PROMISC_PHYS)");

   if (dlpromisconreq(fd, DL_PROMISC_MULTI, ebuf) < 0 || dlokack(fd, "promisc_multi", (char *)buf, ebuf) < 0)
      DEBUG_MSG("WARNING: DL_PROMISC_MULTI failed (%s)", ebuf);


   if ( (dlpromisconreq(fd, DL_PROMISC_SAP, ebuf) < 0 || dlokack(fd, "promisc_sap", (char *)buf, ebuf) < 0))
      ERROR_MSG("dlpromisconreq(DL_PROMISC_SAP)");

   /*
   ** Determine link type
   */
   if (dlinforeq(fd, ebuf) < 0 || dlinfoack(fd, (char *)buf, ebuf) < 0)
      ERROR_MSG("dlinforeq()");


   infop = &((union DL_primitives *)buf)->info_ack;
   switch (infop->dl_mac_type) {
      case DL_CSMACD:
      case DL_ETHER:
         offset = 2;
         break;

      default:
         Error_msg("Interface not supported ( only DLT_EN10MB) | %d", infop->dl_mac_type);
   }

#ifdef DLIOCRAW
   /*
   ** This is a non standard SunOS hack to get the ethernet header.
   */
   if (strioctl(fd, DLIOCRAW, 0, NULL) < 0)
      ERROR_MSG("strioctl(DLIOCRAW)");

#endif

#ifdef HAVE_SYS_BUFMOD_H
   /*
   ** Another non standard call to get the data nicely buffered
   */
   if (ioctl(fd, I_PUSH, "bufmod") != 0)
      ERROR_MSG("ioclt(I_PUSH)");

   /*
   ** Now that the bufmod is pushed lets configure it.
   **
   ** There is a bug in bufmod(7). When dealing with messages of
   ** less than snaplen size it strips data from the beginning not
   ** the end.
   **
   ** This bug is supposed to be fixed in 5.3.2. Also, there is a
   ** patch available. Ask for bugid 1149065.
   */

   ss = MAXDLBUF;

   release = get_release(&osmajor, &osminor, &osmicro);
   if (osmajor == 5 && (osminor <= 2 || (osminor == 3 && osmicro < 2)) && getenv("BUFMOD_FIXED") == NULL) {
      fprintf(stderr, "WARNING: bufmod is broken in SunOS %s; ignoring snaplen.\n", release);
      ss = 0;
   }


   if (ss > 0 && strioctl(fd, SBIOCSSNAP, sizeof(ss), (char *)&ss) != 0)
      ERROR_MSG("strioctl(SBIOCSSNAP)");

   /*
   ** Set up the bufmod flags
   */
   if (strioctl(fd, SBIOCGFLAGS, sizeof(flag), (char *)&flag) < 0)
      ERROR_MSG("strioctl(SBIOCGFLAGS)");

   flag |= SB_NO_DROPS;
   if (strioctl(fd, SBIOCSFLAGS, sizeof(flag), (char *)&flag) != 0)
      ERROR_MSG("strioctl(SBIOCSFLAGS)");

   /*
    * Set the bufmod timeout
    */

   to.tv_sec = 1 / 1000;
   to.tv_usec = (1 * 1000) % 1000000;
   if (strioctl(fd, SBIOCSTIME, sizeof(to), (char *)&to) != 0)
      ERROR_MSG("strioctl(SBIOCSTIME)");

#endif

   /*
   ** As the last operation flush the read side.
   */
   if (ioctl(fd, I_FLUSH, FLUSHR) != 0)
      ERROR_MSG("ioctl(I_FLUSH)");

   bufsize = MAXDLBUF * sizeof(u_int32);             // global var...

   DEBUG_MSG("Inet_OpenRawSock \t fd = %d", fd);

   dlpi_in_use = fd;   // for later use in the Set_Promisc

   return (fd);
}

int
send_request(int fd, char *ptr, int len, char *what, char *ebuf)
{
   struct strbuf ctl;
	int flags;

   ctl.maxlen = 0;
   ctl.len = len;
   ctl.buf = ptr;

	flags = 0;

   if (putmsg(fd, &ctl, (struct strbuf *) NULL, flags) < 0) {
      sprintf(ebuf, "send_request: putmsg \"%s\": %s", what, strerror(errno));
      DEBUG_MSG("\tDLPI : send_request -- ERROR %s", ebuf);
      return (-1);
   }
   return (0);
}

static int
recv_ack(int fd, int size, const char *what, char *bufp, char *ebuf)
{
   union DL_primitives *dlp;
   struct strbuf ctl;
   int flags;

   ctl.maxlen = MAXDLBUF;
   ctl.len = 0;
   ctl.buf = bufp;

   flags = 0;
   if (getmsg(fd, &ctl, (struct strbuf*)NULL, &flags) < 0) {
      sprintf(ebuf, "recv_ack: %s getmsg: %s", what, strerror(errno));
      DEBUG_MSG("\tDLPI : recv_ack -- ERROR %s", ebuf);
      return (-1);
   }

    dlp = (union DL_primitives *)ctl.buf;
    switch (dlp->dl_primitive) {
        case DL_INFO_ACK:
        case DL_PHYS_ADDR_ACK:
        case DL_BIND_ACK:
        case DL_OK_ACK:
#ifdef DL_HP_PPA_ACK
        case DL_HP_PPA_ACK:
#endif
        /*
         *  These are OK
         */
        break;

        case DL_ERROR_ACK:
            switch (dlp->error_ack.dl_errno)
            {
                case DL_BADPPA:
                    sprintf(ebuf, "recv_ack: %s bad ppa (device unit)", what);
                    break;
                case DL_SYSERR:
                    sprintf(ebuf, "recv_ack: %s: %s", what, strerror(dlp->error_ack.dl_unix_errno));
                    break;
                case DL_UNSUPPORTED:
                    sprintf(ebuf, "recv_ack: %s: Service not supplied by provider", what);
                    break;
                default:
                    sprintf(ebuf, "recv_ack: %s error 0x%x", what, (u_int)dlp->error_ack.dl_errno);
                    break;
            }
            return (-1);

        default:
            sprintf(ebuf, "recv_ack: %s unexpected primitive ack 0x%x ", what, (u_int)dlp->dl_primitive);
            return (-1);
    }

    if (ctl.len < size)
    {
        sprintf(ebuf, "recv_ack: %s ack too small (%d < %d)", what, ctl.len, size);
        return (-1);
    }
    return (ctl.len);
}

#endif