#ifndef LMAP_ADDR_H
#define LMAP_ADDR_H

enum {
   MAX_ADDR_LEN = 16,
};

struct ip_addr {
   int type;
   size_t addr_size;
   char addr[MAX_ADDR_LEN];
};

extern int init_addr(struct ip_addr *sa, int type, size_t size, char *addr);
extern const char *print_addr(struct ip_addr *sa, char *buf, size_t size);
extern int get_addr_details(struct ip_addr *sa, char *buf, size_t size);

#endif

/* EOF */

// vim:ts=3:expandtab

