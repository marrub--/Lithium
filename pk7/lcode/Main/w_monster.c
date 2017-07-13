// NB: The style in this file is different. Instead of using Allman style,
//     K&R style is used due to the density of this code.

#include "lith_monster.h"
#include "lith_player.h"
#include "lith_world.h"
#include <math.h>

#define HasResistances(m) ((m)->rank >= 2)
#define MAXRANK 5
#define MAXLEVEL 100


//----------------------------------------------------------------------------
// Types
//

struct dminfo {
   fixed x, y, z;
   fixed r, h;
   int health;
   int lastlevel;
};


//----------------------------------------------------------------------------
// Static Objects
//

static struct {
   enum mtype type;
   __str name;
} const searchnames[] = {
   // Doom 2
   mtype_zombie,        "ZombieMan",
   mtype_zombie,        "ShotgunGuy",
   mtype_zombie,        "ChaingunGuy",
   mtype_imp,           "Imp",
   mtype_demon,         "Demon",
   mtype_demon,         "Spectre",
   mtype_lostsoul,      "LostSoul",
   mtype_mancubus,      "Fatso",
   mtype_mancubus,      "Mancubus",
   mtype_arachnotron,   "Arachnotron",
   mtype_cacodemon,     "Cacodemon",
   mtype_hellknight,    "Knight",
   mtype_baron,         "Baron",
   mtype_revenant,      "Revenant",
   mtype_painelemental, "PainElemental",
   mtype_archvile,      "Archvile",
   mtype_mastermind,    "SpiderMastermind",
   mtype_cyberdemon,    "Cyberdemon",
   
   // Heretic
   mtype_imp,         "Gargoyle",
   mtype_demon,       "Golem",
   mtype_demon,       "Nitrogolem",
   mtype_demon,       "Sabreclaw",
   mtype_mancubus,    "Disciple",
   mtype_arachnotron, "Ophidian",
   mtype_hellknight,  "Warrior",
   mtype_cacodemon,   "IronLich",
   mtype_mastermind,  "Maulotaur",
   mtype_cyberdemon,  "DSparil",
   
   // Lithium
   mtype_phantom, "James",
   mtype_phantom, "Makarov",
   mtype_phantom, "Isaac"
};


//----------------------------------------------------------------------------
// Extern Objects
//

__str const dmgtype_names[dmgtype_max] = {
   "Bullets",
   "Energy",
   "Fire",
   "Magic",
   "Melee",
   "Shrapnel"
};


//----------------------------------------------------------------------------
// Static Functions
//

//
// WaitForResurrect
//
[[__call("SScriptS")]]
static void WaitForResurrect(dmon_t *m)
{
   while(ACS_GetActorProperty(0, APROP_Health) <= 0)
      ACS_Delay(2);
   
   LogDebug(log_dmon, "monster %i resurrected", m->id);
}

//
// GetInfo
//
static void GetInfo(struct dminfo *mi)
{
   mi->x = ACS_GetActorX(0);
   mi->y = ACS_GetActorY(0);
   mi->z = ACS_GetActorZ(0);
   
   mi->r = ACS_GetActorPropertyFixed(0, APROP_Radius);
   mi->h = ACS_GetActorPropertyFixed(0, APROP_Height);
   
   mi->health = ACS_GetActorProperty(0, APROP_Health);
}

//
// ApplyLevels
//
static void ApplyLevels(dmon_t *m, int prev)
{
   GetInfo(m->mi);
   
   for(int i = prev + 1; i <= m->level; i++) {
      if(i % 10 == 0 && HasResistances(m)) {
         int r;
         do
            r = ACS_Random(1, dmgtype_max)-1;
         while(m->resist[r] == 0);
         m->resist[r] += 2;
      }
   }
   
   ACS_SetActorProperty(0, APROP_Health, m->mi->health + (m->level - prev) * m->rank);
   
   for(int i = 0; i < dmgtype_max; i++) {
      ifauto(int, resist, m->resist[i] / 15.0)
         ACS_GiveInventory(StrParam("Lith_M_%S%i", dmgtype_names[i], min(resist, MAXRANK)), 1);
   }
}

//
// ShowBarrier
//
static void ShowBarrier(dmon_t const *m, fixed alpha)
{
   ACS_GiveInventory("Lith_MonsterBarrierLook", 1);
   
   for(int i = 0; i < world.a_cur; i++) {
      struct polar *a = &world.a_angles[i];
      
      fixed dst = m->mi->r / 2 + a->dst / 4;
      fixed x   = m->mi->x + ACS_Cos(a->ang) * dst;
      fixed y   = m->mi->y + ACS_Sin(a->ang) * dst;
      int   tid = ACS_UniqueTID();
      __str bar = m->rank >= 5 ? "Lith_MonsterHeptaura" : "Lith_MonsterBarrier";
      
      ACS_SpawnForced(bar, x, y, m->mi->z + m->mi->h / 2, tid);
      ACS_SetActorPropertyFixed(tid, APROP_Alpha, (1 - a->dst / (256 * (m->rank - 1))) * alpha);
   }
}

//
// BaseMonsterLevel
//
static void BaseMonsterLevel(dmon_t *m)
{
   fixed rn1 = ACS_RandomFixed(1, MAXRANK);
   fixed rn2 = ACS_RandomFixed(1, MAXLEVEL);
   fixed bias;

   switch(world.game) {
   case Game_Episodic: bias = world.mapscleared / 8.0;  break;
   default:            bias = world.mapscleared / 30.0; break;
   }
   
   bias += (ACS_GameSkill() / (fixed)skill_nightmare) * 0.1;
   bias += world.difficulty / 100.0;
   bias *= ACS_RandomFixed(1, 1.5);
   
   m->rank  = minmax(rn1 * bias * 2, 1, MAXRANK);
   m->level = minmax(rn2 * bias * 1, 1, MAXLEVEL);
   
   if(HasResistances(m)) {
      for(int i = 0; i < m->rank; i++)
         m->resist[ACS_Random(1, dmgtype_max)-1] += 5;
   }
   
   ApplyLevels(m, 0);
}

//
// SoulCleave
//
// Spawn a Monster Soul and temporarily set the species of it until the
// actor is no longer solid, so it won't explode immediately.
//
[[__call("ScriptS")]]
static void SoulCleave(dmon_t *m, player_t *p)
{
   int tid = ACS_UniqueTID();
   ACS_SpawnForced("Lith_MonsterSoul", m->mi->x, m->mi->y, m->mi->z + 16, tid);
   
   Lith_SetPointer(tid, AAPTR_DEFAULT, AAPTR_TARGET, p->tid);
   ACS_SetActorPropertyString(tid, APROP_Species, ACS_GetActorPropertyString(0, APROP_Species));
   
   for(int i = 0; ACS_CheckFlag(0, "SOLID") && i < 15; i++)
      ACS_Delay(1);
   
   ACS_SetActorPropertyString(tid, APROP_Species, "Lith_Player");
}

//
// OnDeath
//
static void OnDeath(dmon_t *m)
{
   LogDebug(log_dmon, "monster %i is ded", m->id);
   m->wasdead = true;
   
   ifauto(player_t *, p, Lith_GetPlayer(0, AAPTR_TARGET)) {
      if(p->sigil.acquired && m->type == mtype_imp && m->level >= 50 && m->rank >= 4)
         ACS_SpawnForced("Lith_ClawOfImp", m->mi->x, m->mi->y, m->mi->z);
      
      if(p->getUpgr(UPGR_SoulCleaver)->active)
         SoulCleave(m, p);
   }
   
   Lith_GiveAllScore(m->rank * 500, false);
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_MonsterMain
//
[[__call("ScriptS")]]
void Lith_MonsterMain(dmon_t *m)
{
   ACS_GiveInventory("Lith_MonsterID", m->id + 1);

   struct dminfo mi;
   m->mi = &mi;
   
   BaseMonsterLevel(m);
   LogDebug(log_dmonV, "monster %i\t\Cdr%i \Cgl%i\C-\trunning on %S",
      m->id, m->rank, m->level, ACS_GetActorClass(0));
   
   for(;;) {
      GetInfo(m->mi);
      
      if(mi.health <= 0) {
         OnDeath(m);
         WaitForResurrect(m);
      }
      
      if(HasResistances(m))
         ShowBarrier(m, m->level / (fixed)MAXLEVEL);
      
      ACS_Delay(1);
   }
}

//
// Lith_MonsterInfo
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_MonsterInfo()
{
   __str cname = ACS_GetActorClass(0);
   
   for(int i = 0; i < countof(searchnames); i++) {
      if(strstr_str(cname, searchnames[i].name)) {
         ifauto(dmon_t *, m, AllocDmon()) {
            m->type = searchnames[i].type;
            Lith_MonsterMain(m);
         }
         return;
      }
   }
   
   if(ACS_CheckFlag(0, "COUNTKILL"))
      LogDebug(log_dmon, "invalid monster %S", cname);
   
   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   ACS_GiveInventory("Lith_MonsterInvalid", 1);
}

// EOF

