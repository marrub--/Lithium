// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Phantom and boss handling.                                               │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

struct boss
{
   char const name[16];
   i32  const phasenum;

   i32  phase;
   bool dead;
};

enum
{
   diff_easy,
   diff_medi,
   diff_hard,
   diff_any,
   diff_max
};

static
struct boss bosses_easy[] = {
   {"James", 2},
};

static
struct boss bosses_medi[] = {
   {"Makarov", 3},
};

static
struct boss bosses_hard[] = {
   {"Isaac", 3},
};

static
struct boss *lmvar boss;

static
bool alldead[diff_max];

static
i32 rewardnum, difficulty;

static
struct boss *lastboss;

static
i96 scorethreshold = 1000000;

static
void SpawnBossReward(void) {
   k32 x = GetX(0);
   k32 y = GetY(0);
   k32 z = GetZ(0);

   switch(rewardnum++) {
   case 0: ACS_SpawnForced(so_BossReward1, x, y, z); break;
   case 1: ACS_SpawnForced(so_BossReward2, x, y, z); break;
   case 2: ACS_SpawnForced(so_BossReward3, x, y, z); break;
   case 3: ACS_SpawnForced(so_BossReward4, x, y, z); break;
   case 4: ACS_SpawnForced(so_BossReward5, x, y, z); break;
   case 7: ACS_SpawnForced(so_BossReward6, x, y, z); break;
   }
}

static
void TriggerBoss(void) {
   static
   bool firstboss = true;

   if(!boss) return;

   if(boss->dead) {
      Dbg_Err(_p((cstr)boss->name), _l(" is dead, invalid num"));
      boss = nil;
      return;
   }

   if(boss->phase > boss->phasenum) {
      Dbg_Err(_l("invalid boss phase"));
      boss = nil;
      return;
   }

   if(!boss->phase) {
      boss->phase = 1;
   }

   Dbg_Log(log_boss,
           _l("Spawning boss "), _p((cstr)boss->name), _l(" phase "),
           _p(boss->phase));

   ServCallV(sm_TriggerBoss);

   if(firstboss) {
      firstboss = false;
      P_BIP_Unlock(P_BIP_NameToPage("MPhantom"), false);
   }
}

static
bool CheckDead(struct boss *b, i32 num) {
   for(i32 i = 0; i < num; i++) {
      if(!b[i].dead) {
         return false;
      }
   }
   return true;
}

script
void SpawnBosses(i96 sum, bool force) {
   if(ml.islithmap || (!force && sum < scorethreshold)) return;

   alldead[diff_easy] = CheckDead(bosses_easy, countof(bosses_easy));
   alldead[diff_medi] = CheckDead(bosses_medi, countof(bosses_medi));
   alldead[diff_hard] = CheckDead(bosses_hard, countof(bosses_hard));

   i32 diff =
      difficulty == diff_any ? ACS_Random(diff_easy, diff_hard) : difficulty;

   if(alldead[diff]) {
      Dbg_Log(log_boss, _l("All bosses dead, returning"));
      return;
   }

   Dbg_Log(log_boss, _l("Spawning boss, difficulty "), _p(diff));

   if(!lastboss || lastboss->dead) {
      switch(diff) {
      case diff_easy: do boss = &bosses_easy[ACS_Random(1, countof(bosses_easy)) - 1]; while(boss->dead); break;
      case diff_medi: do boss = &bosses_medi[ACS_Random(1, countof(bosses_medi)) - 1]; while(boss->dead); break;
      case diff_hard: do boss = &bosses_hard[ACS_Random(1, countof(bosses_hard)) - 1]; while(boss->dead); break;
      }
   } else {
      boss = lastboss;
   }

   TriggerBoss();
}

script_str ext("ACS") addr(OBJ "PhantomSound")
void Z_PhantomSound(void) {
   AmbientSound(ss_enemies_phantom_spawned, 0.5k);
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "PhantomTeleport")
void Z_PhantomTeleport(void) {
   k32 ang = ACS_GetActorAngle(0);

   ACS_ThrustThing(ang * 256, 64, true, 0);

   for(i32 i = 0; i < 15; i++) {
      ServCallV(sm_PhantomTeleport);
      ACS_Delay(1);
   }
}

dynam_aut script_str ext("ACS") addr(OBJ "PhantomDeath")
void Z_PhantomDeath(void) {
   ACS_StopSound(0, 7);

   if(boss->phase == boss->phasenum) {
      /* Death */
      AmbientSound(ss_player_death1, 0.5k);
      ACS_Delay(35);
      ServCallV(sm_PlayerDeath);
      ACS_Delay(25);
      ServCallV(sm_PlayerDeathNuke);
      ACS_Delay(25);
      noinit static
      char tag[20];
      faststrcpy3(tag, "M", boss->name, "Defeated");
      P_BIP_Unlock(P_BIP_NameToPage(tag), false);
      boss->dead = true;

      if(difficulty != diff_any) difficulty++;
   } else {
      /* Escape */
      AmbientSound(ss_enemies_phantom_escape, 0.5k);
      ACS_SetActorState(0, sm_GetOutOfDodge);
      ACS_Delay(5);
      ServCallV(sm_PhantomOut);
      ACS_Delay(2);
   }

   Dbg_Log(log_boss,
           _l("Boss "), _p((cstr)boss->name), _l(" phase "), _p(boss->phase),
           _l(" defeated"));

   if(!CVarGetI(sc_sv_nobossdrop)) {
      SpawnBossReward();
   }

   wl.soulsfreed++;

   scorethreshold = scorethreshold * 17 / 10;
   Dbg_Note(_l("score threshold raised to "), _p(scorethreshold), _c('\n'));

   boss->phase++;
   boss = nil;

   wl.bossspawned = false;
}

script_str ext("ACS") addr(OBJ "SpawnBoss")
void Z_SpawnBoss(void) {
   if(!boss) return;

   ServCallV(sm_SpawnBoss, fast_strdup2(OBJ "Boss_", boss->name), boss->phase);

   Dbg_Log(log_boss,
           _l("Boss "), _p((cstr)boss->name), _l(" phase "), _p(boss->phase),
           _l(" spawned"));
   Dbg_Note(_l("boss: "), _p((cstr)boss->name), _l(" phase "), _p(boss->phase),
            _l(" spawned\n"));

   wl.bossspawned = true;
}

script static
bool chtf_summon_boss(cheat_params_t const params) {
   i32 diff  = params[0] - '0';
   i32 num   = params[1] - '0';
   i32 phase = params[2] - '0';

   if(diff < 0 || diff >= diff_any ||
      num < 0 || num > 9 ||
      phase < 0 || phase > 9)
   {
      return false;
   }

   switch(params[0] - '0') {
   case diff_easy: boss = &bosses_easy[num]; break;
   case diff_medi: boss = &bosses_medi[num]; break;
   case diff_hard: boss = &bosses_hard[num]; break;
   }

   if(phase) {
      boss->dead = false;
      boss->phase = phase;
   }

   TriggerBoss();
   return true;
}

struct cheat cht_summon_boss = cheat_s("pghauntme", 3, chtf_summon_boss, "Haunted house initiated");

/* EOF */
