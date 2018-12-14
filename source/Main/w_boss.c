// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

// Types ---------------------------------------------------------------------|

struct boss
{
   __str const name;
   int   const phasenum;

   int  phase;
   bool dead;

   list_t link;
};

enum
{
   diff_easy,
   diff_medi,
   diff_hard,
   diff_any,
   diff_max
};

// Static Objects ------------------------------------------------------------|

static struct boss bosses_easy[] = {
   {"James",   2},
};

static struct boss bosses_medi[] = {
   {"Makarov", 3},
};

static struct boss bosses_hard[] = {
   {"Isaac",   3},
};

static struct boss *lmvar boss;
static int          lmvar bosstid;

static bool alldead[diff_max];

static int rewardnum;
static int difficulty;
static struct boss *lastboss;
static i96 scorethreshold = 1000000;

// Static Functions ----------------------------------------------------------|

static void Lith_SpawnBossReward(void)
{
   fixed x = GetX(0);
   fixed y = GetY(0);
   fixed z = GetZ(0);

   switch(rewardnum++) {
   case 0: ACS_SpawnForced(OBJ "BossReward1", x, y, z); break;
   case 1: ACS_SpawnForced(OBJ "BossReward2", x, y, z); break;
   case 2: ACS_SpawnForced(OBJ "BossReward3", x, y, z); break;
   case 3: ACS_SpawnForced(OBJ "BossReward4", x, y, z); break;
   case 4: ACS_SpawnForced(OBJ "BossReward5", x, y, z); break;
   case 7: ACS_SpawnForced(OBJ "BossReward6", x, y, z); break;
   }
}

static void Lith_TriggerBoss(void)
{
   static bool firstboss = true;

   if(!boss) return;

   if(boss->dead) {
      Log("Lith_TriggerBoss: %S is dead, invalid num", boss->name);
      boss = null;
      return;
   }

   if(boss->phase > boss->phasenum) {
      Log("Lith_TriggerBoss: invalid boss phase");
      boss = null;
      return;
   }

   if(!boss->phase)
      boss->phase = 1;

   LogDebug(log_boss, "Lith_TriggerBoss: Spawning boss %S phase %i", boss->name, boss->phase);

   ServCallI("TriggerBoss");

   if(firstboss) {
      firstboss = false;
      Lith_ForPlayer()
         p->deliverMail("Phantom");
   }
}

static bool CheckDead(struct boss *b, int num)
{
   for(int i = 0; i < num; i++)
      if(!b[i].dead) return false;
   return true;
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_PhantomSound(void)
{
   ACS_AmbientSound("enemies/phantom/spawned", 127);
}

script ext("ACS")
void Lith_PhantomTeleport(void)
{
   fixed ang = ACS_GetActorAngle(0);

   ACS_ThrustThing(ang * 256, 64, true, 0);

   for(int i = 0; i < 15; i++) {
      InvGive(OBJ "PhantomTeleport", 1);
      ACS_Delay(1);
   }
}

script ext("ACS")
void Lith_PhantomDeath(void)
{
   ACS_StopSound(0, 7);

   if(boss->phase == boss->phasenum)
   {
      // Death
      ACS_AmbientSound("player/death1", 127);
      ACS_Delay(35);
      InvGive(OBJ "PlayerDeath", 1);
      ACS_Delay(25);
      InvGive(OBJ "PlayerDeathNuke", 1);
      ACS_Delay(25);
      Lith_ForPlayer() p->deliverMail(StrParam("%SDefeated", boss->name));
      boss->dead = true;

      if(difficulty != diff_any) difficulty++;
   }
   else
   {
      // Escape
      ACS_AmbientSound("enemies/phantom/escape", 127);
      ACS_SetActorState(0, "GetOutOfDodge");
      ACS_Delay(5);
      InvGive(OBJ "PhantomOut", 1);
      ACS_Delay(2);
   }

   LogDebug(log_boss, "Lith_PhantomDeath: %S phase %i defeated", boss->name, boss->phase);

   Lith_SpawnBossReward();
   world.soulsfreed++;

   scorethreshold = scorethreshold * 17 / 10;
   DebugNote("score threshold raised to %lli\n", scorethreshold);

   boss->phase++;
   boss = null;

   world.bossspawned = false;
}

script ext("ACS")
void Lith_SpawnBoss(void)
{
   if(!boss) return;

   bosstid = ACS_ActivatorTID();
   bosstid = bosstid ? bosstid : ACS_UniqueTID();

   ServCallI("SpawnBoss", StrParam(OBJ "Boss_%S", boss->name), boss->phase);

   LogDebug(log_boss, "Lith_SpawnBoss: Boss %S phase %i spawned", boss->name, boss->phase);
   DebugNote("boss: %S phase %i spawned\n", boss->name, boss->phase);

   world.bossspawned = true;
}

script ext("ACS") addr("Lith_TriggerBoss") optargs(1)
void Lith_TriggerBoss_Script(int diff, int num, int phase)
{
   switch(diff)
   {
   case diff_easy: boss = &bosses_easy[num]; break;
   case diff_medi: boss = &bosses_medi[num]; break;
   case diff_hard: boss = &bosses_hard[num]; break;
   }

   if(phase)
      {boss->dead = false; boss->phase = phase;}

   Lith_TriggerBoss();
}

script ext("ACS")
void Lith_SpawnBosses(i96 sum, bool force)
{
   if(!force && sum < scorethreshold) return;

   alldead[diff_easy] = CheckDead(bosses_easy, countof(bosses_easy));
   alldead[diff_medi] = CheckDead(bosses_medi, countof(bosses_medi));
   alldead[diff_hard] = CheckDead(bosses_hard, countof(bosses_hard));

   int diff =
      difficulty == diff_any ? ACS_Random(diff_easy, diff_hard) : difficulty;

   if(alldead[diff])
   {
      LogDebug(log_boss, "Lith_SpawnBosses: All dead, returning");
      return;
   }

   LogDebug(log_boss, "Lith_SpawnBosses: Spawning boss, difficulty %i", diff);

   if(!lastboss || lastboss->dead) switch(diff)
   {
   case diff_easy: do boss = &bosses_easy[ACS_Random(1, countof(bosses_easy)) - 1]; while(boss->dead); break;
   case diff_medi: do boss = &bosses_medi[ACS_Random(1, countof(bosses_medi)) - 1]; while(boss->dead); break;
   case diff_hard: do boss = &bosses_hard[ACS_Random(1, countof(bosses_hard)) - 1]; while(boss->dead); break;
   }
   else
      boss = lastboss;

   Lith_TriggerBoss();
}

// EOF

