#ifndef LITH_MONSTER_H
#define LITH_MONSTER_H

#include "lith_common.h"
#include "lith_list.h"

enum mtype {
   mtype_unknown,
   mtype_zombie,
   mtype_imp,
   mtype_demon,
   mtype_lostsoul,
   mtype_mancubus,
   mtype_arachnotron,
   mtype_cacodemon,
   mtype_hellknight,
   mtype_baron,
   mtype_revenant,
   mtype_painelemental,
   mtype_archvile,
   mtype_mastermind,
   mtype_cyberdemon,
   mtype_phantom,
   mtype_max
};

struct dmon {
   bool active;
   int id;
   enum mtype type;
   
   bool wasdead;
   int exp;
   int level;
   int rank;
   
   struct dminfo *mi;
};

__addrdef extern __gbl_arr dmonarr;

typedef struct dmon dmonarr dmon_t;

extern dmon_t dmonalloc[];
extern int dmonarr dmonid;

[[__call("ScriptS"), __optional_args(2)]] dmon_t *DmonPtr(int tid, int ptr);
dmon_t *Dmon(int id);
dmon_t *AllocDmon(void);

#endif//LITH_MONSTER_H

