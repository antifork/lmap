
#ifndef LMAP_FINGERPRINT_H
#define LMAP_FINGERPRINT_H

extern int fingerprint_init(void);
extern char * fingerprint_search(char *m);

enum {
   FINGER_LEN = 28,
   OS_LEN     = 60,
};
   
#endif

/* EOF */

// vim:ts=3:expandtab

