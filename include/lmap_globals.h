
#ifndef LMAP_GLOBALS_H
#define LMAP_GLOBALS_H


struct lmap_options {
   char dump:1;
   char read:1;
   char raw:1;
   char *iface;
   char *dumpfile;
};

struct program_env {
   char *name;
   char *version;
   char *debug_file;
};

struct pcap_env {
   void *pcap;         /* this is a pcap_t */
   void *dump;         /* this is a pcap_dumper_t */
   u_int16 snaplen;
};


struct globals {
   struct lmap_options *options;
   struct program_env *env;
   struct pcap_env *pcap;
   struct stat_env *stat;
};

extern struct globals *gbls;

#define GBLS gbls

#define GBL_OPTIONS        (GBLS->options)
#define GBL_ENV            (GBLS->env)
#define GBL_PCAP           (GBLS->pcap)

#define GBL_PROGRAM        (GBL_ENV->name)
#define GBL_VERSION        (GBL_ENV->version)
#define GBL_DEBUG_FILE     (GBL_ENV->debug_file)


/* exported functions */

extern void globals_alloc(void);

#endif

/* EOF */

// vim:ts=3:expandtab

