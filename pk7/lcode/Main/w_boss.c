#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

// Types ---------------------------------------------------------------------|

struct phantom_s
{
   property x   {get: ACS_GetActorX(->tid)}
   property y   {get: ACS_GetActorY(->tid)}
   property z   {get: ACS_GetActorZ(->tid)}
   property ang {get: ACS_GetActorAngle(->tid)}
   property health {get: ACS_GetActorProperty(->tid, APROP_Health)}
   property meleetime {
      get: ACS_GetUserVariable(->tid, "user_meleetime"),
      set: ACS_SetUserVariable(->tid, "user_meleetime")
   }

   int tid;
};

// Static Objects ------------------------------------------------------------|

static int lmvar bossargs[6];
static int lmvar bosstid;

// Extern Functions ----------------------------------------------------------|

//
// Lith_PhantomMain
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomMain()
{
   struct phantom_s self;

   self.tid = bosstid;

   ACS_AmbientSound("enemies/phantom/spawned", 127);

   for(;;)
   {
      if(Lith_GetPlayerNumber(0, AAPTR_TARGET) == -1)
         ACS_GiveInventory("Lith_PhantomLOSCheck", 1);

      if(!Lith_ValidPointer(0, AAPTR_TARGET))
         ACS_GiveInventory("Lith_PhantomHunt", 1);

      if((ACS_Timer() % 10) == 0)
         ACS_GiveInventory("Lith_PhantomAura", 1);

      // TODO: fix this when david fixes properties
      // 2017-07-16: david still has not fixed properties
      if(self.meleetime)
         self.meleetime = self.meleetime - 1;

      ACS_Delay(1);
   }
}

//
// Lith_PhantomMain2
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomMain2()
{
   struct phantom_s self;

   self.tid = ACS_UniqueTID();

   for(;;)
   {
      if((ACS_Timer() % 10) == 0)
         ACS_GiveInventory("Lith_PhantomAura", 1);

      // TODO: fix this when david fixes properties
      if(self.meleetime)
         self.meleetime = self.meleetime - 1;

      if(self.health <= 0)
         return;

      ACS_Delay(1);
   }
}

//
// Lith_PhantomTargetCheck
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_PhantomTargetCheck()
{
   return ACS_StrCmp(ACS_GetActorClass(0), "Lith_Boss_", 10) == 0;
}

//
// Lith_PhantomUnsetDupe
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomUnsetDupe()
{
   if(ACS_SetActivator(0, AAPTR_MASTER))
      ACS_SetUserVariable(0, "user_spawnedduplicates", ACS_GetUserVariable(0, "user_spawnedduplicates") - 1);
}

//
// Lith_PhantomTarget
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomTarget()
{
   if(!Lith_ValidPointer(bosstid, AAPTR_TARGET))
   {
      int tid = ACS_ActivatorTID();
      bool unique;

      if(!tid) {
         ACS_Thing_ChangeTID(0, tid = ACS_UniqueTID());
         unique = true;
      }

      ACS_SetActivator(bosstid);
      ACS_SetPointer(AAPTR_TARGET, tid);
      ACS_SetActorState(0, "See");

      if(unique)
         ACS_Thing_ChangeTID(tid, 0);
   }
}

//
// Lith_PhantomTeleport
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomTeleport()
{
   fixed ang = ACS_GetActorAngle(0);

   ACS_ThrustThing(ang * 256, 64, true, 0);

   for(int i = 0; i < 15; i++) {
      ACS_GiveInventory("Lith_PhantomTeleport", 1);
      ACS_Delay(1);
   }
}

//
// Lith_PhantomDeath
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PhantomDeath(int num, int phase)
{
   ACS_StopSound(0, 7);

   ACS_AmbientSound("player/death1", 127);

   ACS_Delay(35);

   ACS_NamedTerminate("Lith_PhantomMain", 0);
   ACS_GiveInventory("Lith_PlayerDeath", 1);

   ACS_Delay(25);

   ACS_GiveInventory("Lith_PlayerDeathNuke", 1);

   ACS_Delay(25);

   #define Phase(num, phasenum) \
      case phasenum: \
         world.boss[num - 1][phasenum - 1] = true; \
         LogDebug(log_boss, "Lith_PhantomDeath: Boss " #num " phase " #phasenum " died"); \

   int x = ACS_GetActorX(0);
   int y = ACS_GetActorY(0);
   int z = ACS_GetActorZ(0);
   switch(num)
   {
   case 1:
      switch(phase)
      {
      Phase(1, 1) ACS_SpawnForced("Lith_BossReward1", x, y, z); break;
      Phase(1, 2) ACS_SpawnForced("Lith_BossReward2", x, y, z); break;
      }
      break;

   case 2:
      switch(phase)
      {
      Phase(2, 1) ACS_SpawnForced("Lith_BossReward3", x, y, z); break;
      Phase(2, 2) ACS_SpawnForced("Lith_BossReward4", x, y, z); break;
      Phase(2, 3) ACS_SpawnForced("Lith_BossReward5", x, y, z); break;
      }
      break;

   case 3:
      switch(phase)
      {
      Phase(3, 1) break;
      Phase(3, 2) break;
      Phase(3, 3) ACS_SpawnForced("Lith_BossReward6", x, y, z); break;
      }
      break;
   }

   #undef Phase
}

//
// Lith_SpawnBossArgs1
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SpawnBossArgs1(int special, int arg0, int arg1, int arg2)
{
   bossargs[0] = special;
   bossargs[1] = arg0;
   bossargs[2] = arg1;
   bossargs[3] = arg2;
}

//
// Lith_SpawnBossArgs2
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SpawnBossArgs2(int arg3, int arg4)
{
   bossargs[4] = arg3;
   bossargs[5] = arg4;
}

//
// Lith_SpawnBoss
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SpawnBoss(int num, int phase)
{
   __str names[] = {"Lith_Boss_James", "Lith_Boss_Makarov", "Lith_Boss_Isaac"};

   fixed x = ACS_GetActorX(0);
   fixed y = ACS_GetActorY(0);
   fixed z = ACS_GetActorZ(0);

   fixed angle = ACS_GetActorAngle(0);

   bosstid = ACS_ActivatorTID();
   bosstid = bosstid ? bosstid : ACS_UniqueTID();

   ACS_Thing_Remove(0);

   ACS_SpawnForced(names[num - 1], x, y, z, bosstid, angle);
   ACS_SetThingSpecial(bosstid, bossargs[0], bossargs[1], bossargs[2], bossargs[3], bossargs[4], bossargs[5]);
   ACS_SetUserVariable(bosstid, "user_phase", phase);

   LogDebug(log_boss, "Lith_SpawnBoss: Boss %i phase %i spawned", num, phase);

   switch(phase)
   {
   case 2:
      ACS_SetActorPropertyFixed(bosstid, APROP_DamageMultiplier, 1.5);
      ACS_SetActorProperty(bosstid, APROP_ReactionTime, 4);
      ACS_SetActorProperty(bosstid, APROP_Health, ACS_GetActorProperty(bosstid, APROP_Health) * 1.5);
      Lith_GiveActorInventory(bosstid, "Lith_PhantomPhase2Flags", 1);
      break;
   case 3:
      ACS_SetActorPropertyFixed(bosstid, APROP_DamageMultiplier, 2.0);
      ACS_SetActorProperty(bosstid, APROP_ReactionTime, 2);
      ACS_SetActorProperty(bosstid, APROP_Health, ACS_GetActorProperty(bosstid, APROP_Health) * 2);
      Lith_GiveActorInventory(bosstid, "Lith_PhantomPhase3Flags", 1);
      break;
   }

   world.bossspawned = true;
}

//
// Lith_TriggerBoss
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_TriggerBoss(int num, int phase)
{
   static bool firstboss = true;

   int tid;

   ACS_SpawnForced("Lith_BossSpawner", 0, 0, 0, tid = ACS_UniqueTID());
   ACS_SetActorState(tid, StrParam("Boss%i_%i", num, phase));

   LogDebug(log_boss, "Lith_TriggerBoss: Spawning boss %i phase %i", num, phase);

   if(firstboss) {
      firstboss = false;
      Lith_ForPlayer()
         p->deliverMail("Phantom");
   }
}

//
// Lith_SpawnBosses
//
void Lith_SpawnBosses(score_t sum)
{
   // WHY ARE CONDITIONS SO HARD IT TOOK ME 7 TRIES TO GET THIS RIGHT
   // NOTE: INCREMENT THIS COUNTER EVERY TIME I GET IT WRONG

        if(!world.boss[0][0] &&                     sum > world.boss1p1scr) Lith_TriggerBoss(1, 1);
   else if(!world.boss[0][1] && world.boss[0][0] && sum > world.boss1p2scr) Lith_TriggerBoss(1, 2);
   else if(!world.boss[1][0] && world.boss[0][1] && sum > world.boss2p1scr) Lith_TriggerBoss(2, 1);
   else if(!world.boss[1][1] && world.boss[1][0] && sum > world.boss2p2scr) Lith_TriggerBoss(2, 2);
   else if(!world.boss[1][2] && world.boss[1][1] && sum > world.boss2p3scr) Lith_TriggerBoss(2, 3);
   else if(!world.boss[2][0] && world.boss[1][2] && sum > world.boss3p1scr) Lith_TriggerBoss(3, 1);
   else if(!world.boss[2][1] && world.boss[2][0] && sum > world.boss3p2scr) Lith_TriggerBoss(3, 2);
   else if(!world.boss[2][2] && world.boss[2][1] && sum > world.boss3p3scr) Lith_TriggerBoss(3, 3);
}

// EOF

