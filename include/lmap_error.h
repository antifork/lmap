
#ifndef LMAP_ERROR_H
#define LMAP_ERROR_H

#include <errno.h>

extern void error_msg(char *file, char *function, int line, char *message, ...);

#define ERROR_MSG(x, args...) error_msg(__FILE__, __FUNCTION__, __LINE__, x, ## args)

#define FATAL_MSG(x, args...) do { fprintf(stderr, "\n"x"\n\n", ## args); exit(-1); } while(0)

#define ON_ERROR(x, y, fmt, args...) do { if (x == y) ERROR_MSG(fmt, ## args); } while(0)


#define ESUCCESS     0
#define ENOTFOUND    1
#define ENOTHANDLED  2
#define EINVALID     3
#define ENOADDRESS   4

#endif

/* EOF */

// vim:ts=3:expandtab

