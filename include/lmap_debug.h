
#if defined (DEBUG) && !defined(LMAP_DEBUG_H)
#define LMAP_DEBUG_H

extern void debug_init(void);
extern void debug_msg(char *message, ...);
extern char * hex_format(const u_char *buffer, int buff_len);

extern FILE *debug_file;

#define DEBUG_INIT() debug_init()
#define DEBUG_MSG(x, args...) do {                             \
   if (debug_file == NULL)                                     \
      FATAL_MSG("[%s::%s] DEBUG_MSG called before initialization !!", __FILE__, __FUNCTION__);  \
   else                                                        \
      debug_msg(x, ## args);                                   \
} while(0)

#define USER_MSG DEBUG_MSG

#endif /* LMAP_DEBUG_H */

/* 
 * if DEBUG is not defined we expand the macros to null instructions...
 */

#ifndef DEBUG
   #define DEBUG_INIT()
   #define DEBUG_MSG(x, args...)
   #define USER_MSG(x, args...) fprintf(stdout, x, ## args )
#endif

/* EOF */

// vim:ts=3:expandtab

