
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


#ifndef HAVE_STRLCAT
   #include <missing/strlcat.h>
#endif
#ifndef HAVE_STRLCPY 
   #include <missing/strlcpy.h>
#endif

#define SAFE_FREE(x) do{ if(x) { free(x); x = NULL; } }while(0)

#define __init __attribute__ ((constructor))

#endif   /*  LMAP_H */

extern void do_nothing(void);


/* EOF */

// vim:ts=3:expandtab

