/*
    lmap -- signal handler

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

#include <signal.h>
#include <sys/resource.h>

void signal_handler(void);

RETSIGTYPE signal_SEGV(int sig);
RETSIGTYPE signal_TERM(int sig);


/*************************************/

void signal_handler(void)
{
   DEBUG_MSG("signal_handler activated");

   signal(SIGSEGV,  signal_SEGV);
   signal(SIGINT,   signal_TERM);
   signal(SIGTERM,  signal_TERM);
   signal(SIGCHLD,  SIG_IGN);       /* if I kill a forked process it doesn't become a zombie... */
}


RETSIGTYPE signal_SEGV(int sig)
{
#ifdef DEBUG

   struct rlimit corelimit = {RLIM_INFINITY, RLIM_INFINITY};

   DEBUG_MSG("Segmentation Fault...");
   
   fprintf (stderr, "\n\033[01m\033[1m Ooops !! This shouldn't happen...\n\n");
   fprintf (stderr, "Segmentation Fault...\033[0m\n\n");

   fprintf (stderr, "===========================================================================\n");
   fprintf (stderr, " To report this error follow these steps:\n\n");
   fprintf (stderr, "  1) recompile %s in debug mode : \n"
                    "  \t\"configure --enable-debug && make clean && make\"\n\n", GBL_PROGRAM);
   fprintf (stderr, "  2) reproduce the critical situation\n\n");
   fprintf (stderr, "  3) make a report : \"tar zcvf error.tar.gz %s_debug.log \"\n\n", GBL_PROGRAM);
   fprintf (stderr, "  4) get the gdb backtrace :\n"
                    "  \t - \"gdb %s core\"\n"
                    "  \t - at the gdb prompt \"bt\"\n"
                    "  \t - at the gdb prompt \"quit\" and return to the shell\n"
                    "  \t - copy and paste this output.\n\n", GBL_PROGRAM);
   fprintf (stderr, "  5) mail me the output of gdb and the error.tar.gz\n");
   fprintf (stderr, "============================================================================\n");
   
   fprintf (stderr, "\n\033[01m\033[1m Overriding any 'ulimit -c 0'...\n"
                   " Setting core size to RLIM_INFINITY...\n\n"
                   " Core dumping... (use the 'core' file for gdb analysis)\033[0m\n\n");
   
   /* foce the coredump */
   
   setrlimit(RLIMIT_CORE, &corelimit);
   signal(sig, SIG_DFL);
   raise(sig);
#else
   fprintf(stderr, "Ooops ! This shouldn't happen...");
   fprintf(stderr, "Segmentation fault !");
   fprintf(stderr, "Please recompile in debug mode and send a bugreport");
   
   exit(666);
#endif
}



RETSIGTYPE signal_TERM(int sig)
{
#ifdef HAVE_STRSIGNAL
   DEBUG_MSG("Signal handler... (caught SIGNAL: %d) | %s", sig, strsignal(sig));
#else
   DEBUG_MSG("Signal handler... (caught SIGNAL: %d)", sig);
#endif


   #ifdef HAVE_STRSIGNAL
      fprintf(stderr, "\n\n Shutting down %s (received SIGNAL: %d | %s)\n\n", GBL_PROGRAM, sig, strsignal(sig));
   #else
      fprintf(stderr, "\n\n Shutting down %s (received SIGNAL: %d)\n\n", GBL_PROGRAM, sig);
   #endif

   signal(sig, SIG_IGN);

   exit(1);

}


/* EOF */

// vim:ts=3:expandtab

