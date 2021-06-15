/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Monster tracking and information.
 *
 * ---------------------------------------------------------------------------|
 */
#if defined(monster_flag_x)
monster_flag_x(full)
monster_flag_x(angelic)
monster_flag_x(dark)
#undef monster_flag_x
#elif defined(monster_type_x)
monster_type_x(zombie)
monster_type_x(zombiesg)
monster_type_x(zombiecg)
monster_type_x(imp)
monster_type_x(demon)
monster_type_x(lostsoul)
monster_type_x(mancubus)
monster_type_x(arachnotron)
monster_type_x(cacodemon)
monster_type_x(hellknight)
monster_type_x(baron)
monster_type_x(revenant)
monster_type_x(painelemental)
monster_type_x(archvile)
monster_type_x(mastermind)
monster_type_x(cyberdemon)
monster_type_x(phantom)
monster_type_x(angel)
monster_type_x(darkone)
#undef monster_type_x
#elif !defined(w_monster_h)
#define w_monster_h

#if !ZscOn
#include "common.h"
#include "m_list.h"

enum {
   _monster_level_exp = 777,
};

enum dmgtype {
   dmgtype_bullets,
   dmgtype_energy,
   dmgtype_fire,
   dmgtype_magic,
   dmgtype_melee,
   dmgtype_shrapnel,
   dmgtype_max
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

   k32  x, y, z;
   k32  r, h;
   i32  health;
   i32  painwait;
   bool finalized;
   bool resurrect;

   struct monster_info const *mi;
};

/* this must be kept as a typedef, because it could be moved to another */
/* address space, and then I'd have to do a bunch of rewriting again */
typedef struct dmon dmon_t;

script dmon_t *DmonPtr(i32 tid, i32 ptr);
dmon_t *DmonSelf(void);
dmon_t *Dmon(i32 id);
stkcall dmon_t *AllocDmon(void);

void PrintMonsterInfo(dmon_t *m);
void PrintDmonAllocSize(void);

script void Mon_Init(void);
#endif

enum ZscName(MonsterType, mtype) {
   mtype_unknown,
   #define monster_type_x(name) mtype_##name,
   #include "w_monster.h"
   mtype_max
};

#endif
