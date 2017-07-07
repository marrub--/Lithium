#include "lith_monster.h"

static __str searchnames[] = {
   "ZombieMan",
   "ShotgunGuy",
   "ChaingunGuy",
   "Imp",
   "Demon",
   "Spectre",
   "LostSoul",
   "Fatso",
   "Arachnotron",
   "Cacodemon",
   "HellKnight",
   "Revenant",
   "PainElemental",
   "Archvile",
   "SpiderMastermind",
   "Cyberdemon"
};

// This is lazy-allocated. Don't touch.
[[__no_init]] dmon_t dmonalloc[DMON_MAX];
int lwvar curid;

[[__call("ScriptS")]]
dmon_t *DmonPtr(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   ifauto(int, id, ACS_CheckInventory("Lith_MonsterID"))
      return Dmon(id - 1);
   else
      return null;
}

dmon_t *Dmon(int id)
{
   if(dmonalloc[id].active)
      return &dmonalloc[id];
   else
      return null;
}

dmon_t *AllocDmon(void)
{
   dmon_t *m = &dmonalloc[curid];
   *m = (struct dmon){};
   
   m->active = true;
   m->id = curid;
   
   curid++;
   
   return m;
}

[[__call("SScriptS")]]
static void WaitResurrect(dmon_t *m)
{
   while(ACS_GetActorProperty(0, APROP_Health) <= 0)
      ACS_Delay(1);
   
   LogDebug(log_dmon, "monster %i resurrected", m->id);
}

[[__call("ScriptS")]]
void Lith_MonsterMain(dmon_t *m)
{
   ACS_GiveInventory("Lith_MonsterID", m->id + 1);
   m->level = ACS_Random(0, 100);
   
   LogDebug(log_dmon, "monster %i running on %S", m->id, ACS_GetActorClass(0));
   
   for(;;) {
      if(ACS_GetActorProperty(0, APROP_Health) <= 0) {
         LogDebug(log_dmon, "monster %i is dead", m->id);
         WaitResurrect(m);
      }
      
      ACS_Delay(1);
   }
}

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
   
   // If the monster failed all checks, give them this so we don't need to recheck every tick.
   ACS_GiveInventory("Lith_MonsterInvalid", 1);
}

// EOF

