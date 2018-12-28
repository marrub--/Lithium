// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef w_monster_h
#define w_monster_h

#include "common.h"
#include "m_list.h"

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
   str        name;
   i32        flags;
};

struct dmon {
   bool active;
   u32  id;

   bool wasdead;
   i32  exp;
   i32  level;
   i32  rank;
   i32  maxhealth;
   i32  spawnhealth;
   i32  resist[dmgtype_max];

   struct dmon_stat          *ms;
   struct monster_info const *mi;
};

typedef struct dmon dmon_t;

script dmon_t *DmonPtr(i32 tid, i32 ptr);
stkcall dmon_t *DmonSelf(void);
stkcall dmon_t *Dmon(u32 id);
dmon_t *AllocDmon(void);

void PrintDmonAllocSize(struct player *p);

#endif
