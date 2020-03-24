/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Monster tracking and information.
 *
 * ---------------------------------------------------------------------------|
 */
/* decompat-out pk7/lzscript/Constants/w_monster.zsc */
/* decompat-cut */
#ifndef w_monster_h
#define w_monster_h

#include "common.h"
#include "m_list.h"

enum dmgtype {
   dmgtype_bullets,
   dmgtype_energy,
   dmgtype_fire,
   dmgtype_magic,
   dmgtype_melee,
   dmgtype_shrapnel,
   dmgtype_max
};
/* decompat-end */

enum mtype /* Lith_MonsterType*/ {
   mtype_unknown,
   mtype_zombie,
   mtype_zombiesg,
   mtype_zombiecg,
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
   mtype_angel,
   mtype_darkone,
   mtype_max
};

/* decompat-cut */
struct monster_info {
   u64        exp;
   i96        score;
   enum mtype type;
   str        name;
   i32        flags;
};

struct dmon {
   bool active;
   i32  id;

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

/* this must be kept as a typedef, because it could be moved to another */
/* address space, and then I'd have to do a bunch of rewriting again */
typedef struct dmon dmon_t;

script dmon_t *DmonPtr(i32 tid, i32 ptr);
stkcall dmon_t *DmonSelf(void);
stkcall dmon_t *Dmon(i32 id);
dmon_t *AllocDmon(void);

void PrintDmonAllocSize(struct player *p);

#endif
/* decompat-end */
