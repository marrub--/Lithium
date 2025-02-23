// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Monster tracking and information.                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(monster_flag_x)
monster_flag_x(full)
monster_flag_x(nocase)
monster_flag_x(angelic)
monster_flag_x(dark)
monster_flag_x(nonmonster)
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
monster_type_x(person)
#undef monster_type_x
#elif !defined(w_monster_h)
#define w_monster_h

enum ZscName(MonsterType, mtype) {
   mtype_unknown,
   #define monster_type_x(name) mtype_##name,
   #include "w_monster.h"
   mtype_max
};

enum ZscName(LicariGender, lic_gender) {
   _lic_ocean,
   _lic_earth,
   _lic_sky,
};

enum ZscName(ServGender, srv_gender) {
   _srv_mortal,
   _srv_unmoving,
   _srv_structured,
   _srv_destructured,
};

#if !ZscOn
#include "m_engine.h"
#include "m_list.h"

enum {
   _monster_level_exp = 777,
   _max_rank  = 5,
   _max_level = 150,
};

enum {
   #define monster_flag_x(flg) _mif_##flg,
   #include "w_monster.h"
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

typedef char mon_name_t[64];

struct monster_preset {
   mon_name_t prename;
   i32        exp;
   score_t    score;
};

struct monster_info {
   anonymous struct monster_preset pre;
   i32        type;
   mon_name_t name;
   i32        flags;
   i32        mass;
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
   k32  damagemul;
   i32  resist[dmgtype_max];
   k32  x, y, z;
   k32  r, h;
   i32  health;
   i32  painwait;
   bool finalized;
   bool resurrect;
   struct monster_info const *mi;
};

typedef struct dmon dmon_t;

dmon_t *DmonSelf(void);
dmon_t *Dmon(i32 id);
stkcall dmon_t *AllocDmon(void);
void PrintMonsterInfo(dmon_t *m);
void PrintDmonAllocSize(void);
script void Mon_Init(void);
#endif
#endif
