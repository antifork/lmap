/*
    lmap -- lan discovery tool

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
#include <lmap_version.h>
#include <lmap_signal.h>
#include <lmap_capture.h>
#include <lmap_parser.h>
#include <lmap_threads.h>
#include <lmap_ui.h>
#include <lmap_manuf.h>


/* global vars */


/* protos */

void do_nothing(void);

/*******************************************/

int main(int argc, char *argv[])
{
   /*
    * Alloc the global structures
    * We can access these structs via the macro in sad_globals.h
    */
        
   globals_alloc();
  
   GBL_PROGRAM = strdup(LMAP_PROGRAM);
   GBL_VERSION = strdup(LMAP_VERSION);
   GBL_DEBUG_FILE = calloc(1, strlen(LMAP_PROGRAM) + strlen("_debug.log") + 1);
   ON_ERROR(GBL_DEBUG_FILE, NULL, "can't allocate debug filename");
   sprintf(GBL_DEBUG_FILE, "%s_debug.log", GBL_PROGRAM);
   
   DEBUG_INIT();
   DEBUG_MSG("main -- here we go !!");
   
   /* register the main thread in the thread list*/

   lmap_thread_register(LMAP_SELF, "main", "");
   
   /* activate the signal handler */
   
   signal_handler();

   /*
    * getopt related parsing...
    */
   
   parse_options(argc, argv);
  
   /* initialize the user interface */
   
   ui_init();
   
   USER_MSG("\n\033[01m\033[1m%s %s\033[0m\n\n", GBL_PROGRAM, GBL_VERSION);

   /* initialize libpcap */
   
   capture_init();

   /* load the mac-fingerprints */

   manuf_init();

   /* INITIALIZATION PHASE TERMINATED */
   
   /*
    * go !
    * start capturing packets.
    * this fuction will never return until shutdown
    * make it a thread
    */
   
   lmap_thread_new("capture", "pcap handler and packet decoder", 
                   &capture, NULL);
   
   /* display the User Interface */
   
   lmap_thread_register(LMAP_SELF, "UI", "the user interface");
   ui_start();

   /* reached only whent the UI is shutted down */

   /* kill all the running threads but the current */
   lmap_thread_kill_all();
  
   /* clean up the UI */
   ui_cleanup();

   return 0;
}

/* 
 * this function does nothing.
 * useful to assign a value to a function pointer
 * that MUST NOT be null, but you want to do nothing
 */

void do_nothing(void)
{
   return;
}

/* EOF */


// vim:ts=3:expandtab

