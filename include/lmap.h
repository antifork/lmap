
#ifndef LMAP_H
#define LMAP_H

#ifdef HAVE_CONFIG_H
   #include <config.h>
#endif

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#if !defined (__USE_GNU)  /* for memmem(), strsignal(), etc etc... */
   #define __USE_GNU
#endif
#include <string.h>
#if defined (__USE_GNU)
   #undef __USE_GNU
#endif
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <missing/queue.h>
#include <lmap_error.h>
#include <lmap_debug.h>
#include <lmap_stdint.h>
#include <lmap_globals.h>
#include <lmap_stat.h>
#include <lmap_addr.h>

#ifndef HAVE_STRLCAT
   #include <missing/strlcat.h>
#endif
#ifndef HAVE_STRLCPY 
   #include <missing/strlcpy.h>
#endif

#define SAFE_FREE(x) do{ if(x) { free(x); x = NULL; } }while(0)

#define __init __attribute__ ((constructor))

#ifndef __set_errno
#define __set_errno(e) (errno = (e))
#endif

#define SPRINTF(x) ((size_t)sprintf x)

#define ADD_STAT(x)  (GBL_STAT->x)++
#define GET_STAT(x)  (GBL_STAT->x)


extern void do_nothing(void);


#endif   /*  LMAP_H */

/* EOF */

// vim:ts=3:expandtab

