/*
    lmap -- data handling module (fingerprints databases etc)

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
#include <lmap_data.h>

/* protos */

char * get_full_path(char *file);
char * get_local_path(char *file);
FILE * open_data(char *file, char *mode);

/*******************************************/

/*
 * add the prefix to a given filename
 */

char * get_full_path(char *file)
{
   char *filename;

   filename = calloc(strlen(INSTALL_PREFIX)+strlen(GBL_PROGRAM)+strlen(file)+3, sizeof(char));
   ON_ERROR(filename, NULL, "out of memory");
   
   sprintf(filename, "%s/%s/%s", INSTALL_PREFIX, GBL_PROGRAM, file);

   DEBUG_MSG("get_full_path -- %s", filename);
   
   return filename;
}

/*
 * add the local path to a given filename
 */

char * get_local_path(char *file)
{
   char *filename;

   filename = calloc(strlen("../share/") + strlen(file) + 1, sizeof(char));
   ON_ERROR(filename, NULL, "out of memory");
   
   sprintf(filename, "../share/%s", file);
   
   DEBUG_MSG("get_local_path -- %s", filename);
   
   return filename;
}


/*
 * opens a share file
 */

FILE * open_data(char *file, char *mode)
{
   FILE *fd;
   char *filename = NULL;

   filename = get_full_path(file);
  
   DEBUG_MSG("open_data (%s)", filename);
   
   fd = fopen(filename, mode);
   if (fd == NULL) {
      SAFE_FREE(filename);
      filename = get_local_path(file);

      DEBUG_MSG("open_data dropping to %s", filename);
      
      fd = fopen(filename, mode);
      ON_ERROR(fd, NULL, "can't find %s", filename);
   }
 
   SAFE_FREE(filename);
   
   return fd;
}


/* EOF */

// vim:ts=3:expandtab

