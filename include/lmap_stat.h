
#ifndef LMAP_STAT_H
#define LMAP_STAT_H

extern void register_stat(const char *id);
extern void update_stat(const char *id, const u_int32 count);
extern u_int32 get_stat(const char *type);
extern int get_all_stats(char **type, int *num);
extern void free_stats(void);
extern u_int32 get_stat_count(void);

#endif

/* EOF */

// vim:ts=3:expandtab

