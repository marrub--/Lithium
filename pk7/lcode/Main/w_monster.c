#include "lith_monster.h"
#include "lith_player.h"
#include "lith_world.h"
#include <math.h>


//----------------------------------------------------------------------------
// Types
//

struct dminfo {
   fixed x, y, z;
   fixed r, h;
   int health;
};


//----------------------------------------------------------------------------
// Static Objects
//

static __str searchnames[] = {
   "ZombieMan",
   "ShotgunGuy",
   "ChaingunGuy",
   "Imp",
   "Demon",
   "Spectre",
   "LostSoul",
   "Fatso",
   "Mancubus",
   "Arachnotron",
   "Cacodemon",
   "Knight",
   "Baron",
   "Revenant",
   "PainElemental",
   "Archvile",
   "SpiderMastermind",
   "Cyberdemon",
   "James",
   "Makarov",
   "Isaac"
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
// ShowBarrier
//
static void ShowBarrier(struct dminfo const *mi, fixed alpha)
{
   ACS_GiveInventory("Lith_MonsterBarrierLook", 1);
   
   for(int i = 0; i < world.a_cur; i++) {
      struct polar *a = &world.a_angles[i];
      fixed dst = mi->r / 2 + a->dst / 4;
      fixed x = mi->x + ACS_Cos(a->ang) * dst;
      fixed y = mi->y + ACS_Sin(a->ang) * dst;
      int tid = ACS_UniqueTID();
      ACS_SpawnForced("Lith_MonsterBarrier", x, y, mi->z + mi->h / 2, tid);
      ACS_SetActorPropertyFixed(tid, APROP_Alpha, (1 - a->dst / 256) * alpha);
   }
}

//
// BaseMonsterLevel
//
static void BaseMonsterLevel(dmon_t *m)
{
   fixed rng1 = ACS_RandomFixed(1, 5);
   fixed rng2 = ACS_RandomFixed(1, 100);
   fixed bias;

   switch(world.game) {
   case Game_Episodic: bias = world.mapscleared / 8.0;  break;
   default:            bias = world.mapscleared / 30.0; break;
   }
   
   bias += (ACS_GameSkill() / skill_nightmare) / 2.0;
   
   bias = bias * ACS_RandomFixed(1, 1.5);
   m->rank  = minmax(rng1 * bias * 2, 1, 5);
   m->level = minmax(rng2 * bias * 1, 1, 100);
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
   LogDebug(log_dmonV, "monster %i\t\Cdr%i \Cgl%i\C-\trunning on %S", m->id, m->rank, m->level, ACS_GetActorClass(0));
   
   for(;;) {
      GetInfo(m->mi);
      
      if(mi.health <= 0) {
         OnDeath(m);
         WaitForResurrect(m);
      }
      
      if(m->rank >= 2) {
         ShowBarrier(m->mi, m->level / 100.0);
         // TODO: resistances
      }
      
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
      if(strstr_str(cname, searchnames[i])) {
         ifauto(dmon_t *, m, AllocDmon())
            Lith_MonsterMain(m);
         return;
      }
   }
   
   if(ACS_CheckFlag(0, "COUNTKILL"))
      LogDebug(log_dmon, "invalid monster %S", cname);
   
   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   ACS_GiveInventory("Lith_MonsterInvalid", 1);
}

// EOF

