
#ifndef LMAP_STAT_H
#define LMAP_STAT_H

extern struct stat_env *get_stathead(void);
extern void register_stat(const char *id);
extern void update_stat(const char *id, const u_int32 count);
extern void free_stats(void);
extern u_int32 get_stat_count(void);
extern u_int16 get_max_id_len(void);

#endif

/* EOF */

// vim:ts=3:expandtab

