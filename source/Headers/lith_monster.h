// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef LITH_MONSTER_H
#define LITH_MONSTER_H

#include "lith_common.h"
#include "lith_list.h"

#define MAXRANK 5
#define MAXLEVEL 150

enum dmgtype {
   dmgtype_bullets,
   dmgtype_energy,
   dmgtype_fire,
   dmgtype_magic,
   dmgtype_melee,
   dmgtype_shrapnel,
   dmgtype_max
};

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

struct monster_info {
   u64        exp;
   i96        score;
   enum mtype type;
   __str      name;
   int        flags;
};

struct dmon {
   bool active;
   u32  id;

   bool wasdead;
   int  exp;
   int  level;
   int  rank;
   int  maxhealth;
   int  resist[dmgtype_max];

   struct dmon_stat          *ms;
   struct monster_info const *mi;
};

typedef struct dmon dmon_t;

script dmon_t *DmonPtr(int tid, int ptr);
stkcall dmon_t *DmonSelf(void);
stkcall dmon_t *Dmon(u32 id);
dmon_t *AllocDmon(void);

void PrintDmonAllocSize(struct player *p);

#endif//LITH_MONSTER_H

