
#ifndef LMAP_DATA_H
#define LMAP_DATA_H

extern char * get_full_path(char *file);
extern char * get_local_path(char *file);
extern FILE * open_data(char *file, char *mode);


#define MAC_FINGERPRINTS   "mac-fingerprints"
#define TCP_FINGERPRINTS   "tcp-fingerprints"


#endif

/* EOF */

// vim:ts=3:expandtab

