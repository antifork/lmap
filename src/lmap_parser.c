/*
    lmap -- parsing utilities

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

#ifdef HAVE_GETOPT_H
   #include <getopt.h>
#else
   #include <missing/getopt.h>
#endif

/* protos... */

void lmap_usage(void);
void parse_options(int argc, char **argv);

//-----------------------------------

void lmap_usage(void)
{

   fprintf (stdout, "\nUsage: %s [OPTIONS] \n", GBL_PROGRAM);

   fprintf (stdout, "\nGeneral options:\n");
   fprintf (stdout, "  -i, --iface <iface>          use this interface\n\n");
   fprintf (stdout, "  -v, --version                prints the version and exit\n");
   fprintf (stdout, "  -h, --help                   this help screen\n");

   fprintf(stdout, "\n\n");

   exit (0);
}


void parse_options(int argc, char **argv)
{
   int c;

   static struct option long_options[] = {
      { "help", no_argument, NULL, 'h' },
      { "version", no_argument, NULL, 'v' },
      { "iface", required_argument, NULL, 'i' },
      { 0 , 0 , 0 , 0}
   };

   for (c = 0; c < argc; c++)
      DEBUG_MSG("parse_options -- [%d] [%s]", c, argv[c]);

   optind = 0;

   while ((c = getopt_long (argc, argv, "hvi:", long_options, (int *)0)) != EOF) {

      switch (c) {

         case 'i':
                  GBL_OPTIONS->iface = strdup(optarg);
                  break;
                  
         case 'h':
                  lmap_usage();
                  break;

         case 'v':
                  printf("%s (LAN mapper tool) %s\n", GBL_PROGRAM, GBL_VERSION);
                  exit(0);
                  break;

         case ':': // missing parameter
            fprintf(stdout, "\nTry `%s --help' for more options.\n\n", GBL_PROGRAM);
            exit(0);
         break;

         case '?': // unknown option
            fprintf(stdout, "\nTry `%s --help' for more options.\n\n", GBL_PROGRAM);
            exit(0);
         break;
      }
   }

   return;
}


/* EOF */


// vim:ts=3:expandtab

