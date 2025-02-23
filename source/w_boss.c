// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Phantom and boss handling.                                               │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

struct boss {
   char const name[16];
   i32  const phasenum;

   i32  phase;
   bool dead;
};

enum {
   _diff_easy,
   _diff_medi,
   _diff_hard,
   _diff_any,
   _diff_max
};

static struct boss bosses_easy[] = {
   {"James", 2},
   /*{"Nesse", 1},*/
};

static struct boss bosses_medi[] = {
   {"Makarov", 3},
   /*{"Lily", 3},*/
};

static struct boss bosses_hard[] = {
   {"Isaac", 3},
   /*{"Drench", 3},*/
};

static struct boss *lmvar boss;

static bool alldead[_diff_max];

static i32 rewardnum, difficulty;

static struct boss *lastboss;

stkoff static void set_next_score_threshold() {
   static score_t mul = 17;
   wl.scorethreshold = _scorethreshold_default * mul / 10;
   mul += 10;
}

stkoff static void SpawnBossReward(void) {
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

static stkoff void TriggerBoss(void) {
   static bool firstboss = true;

   if(!boss) return;

   if(boss->dead) {
      PrintErr(_p((cstr)boss->name), _l(" is dead, invalid num"));
      boss = nil;
      return;
   }

   if(boss->phase > boss->phasenum) {
      PrintErr(_l("invalid boss phase"));
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
      P_BIP_UnlockName("MPhantom", true);
   }
}

static stkoff bool CheckDead(struct boss *b, i32 num) {
   for(i32 i = 0; i < num; i++) {
      if(!b[i].dead) {
         return false;
      }
   }
   return true;
}

alloc_aut(0) script void SpawnBosses(score_t sum, bool force) {
   if(get_msk(ml.flag, _mflg_boss) == _mphantom_nospawn || (!force && sum < wl.scorethreshold)) {
      return;
   }

   alldead[_diff_easy] = CheckDead(bosses_easy, countof(bosses_easy));
   alldead[_diff_medi] = CheckDead(bosses_medi, countof(bosses_medi));
   alldead[_diff_hard] = CheckDead(bosses_hard, countof(bosses_hard));

   i32 diff =
      difficulty == _diff_any ? ACS_Random(_diff_easy, _diff_hard) : difficulty;

   if(alldead[diff]) {
      Dbg_Log(log_boss, _l("All bosses dead, returning"));
      return;
   }

   Dbg_Log(log_boss, _l("Spawning boss, difficulty "), _p(diff));

   if(!lastboss || lastboss->dead) {
      switch(diff) {
      case _diff_easy: do boss = &bosses_easy[ACS_Random(1, countof(bosses_easy)) - 1]; while(boss->dead); break;
      case _diff_medi: do boss = &bosses_medi[ACS_Random(1, countof(bosses_medi)) - 1]; while(boss->dead); break;
      case _diff_hard: do boss = &bosses_hard[ACS_Random(1, countof(bosses_hard)) - 1]; while(boss->dead); break;
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

alloc_aut(0) script_str ext("ACS") addr(OBJ "PhantomDeath")
void Z_PhantomDeath(void) {
   ACS_StopSound(0, 7);

   if(boss->phase == boss->phasenum) {
      /* Death */
      StartSound(ss_player_death1, lch_auto, CHANF_NOPAUSE|CHANF_LISTENERZ|CHANF_NOSTOP, 0.5k, ATTN_NONE);
      ACS_Delay(35);
      ServCallV(sm_PlayerDeath);
      ACS_Delay(25);
      ServCallV(sm_PlayerDeathNuke);
      ACS_Delay(25);
      noinit static
      char tag[20];
      faststrcpy3(tag, "M", boss->name, "Defeated");
      P_BIP_UnlockName(tag, true);
      boss->dead = true;

      if(difficulty != _diff_any) difficulty++;
   } else {
      /* Escape */
      StartSound(ss_enemies_phantom_escape, lch_auto, CHANF_NOPAUSE|CHANF_LISTENERZ|CHANF_NOSTOP, 0.5k, ATTN_NONE);
      ACS_SetActorState(0, sm_GetOutOfDodge);
      ACS_Delay(5);
      ServCallV(sm_PhantomOut);
      ACS_Delay(2);
   }

   Dbg_Log(log_boss,
           _l("Boss "), _p((cstr)boss->name), _l(" phase "), _p(boss->phase),
           _l(" defeated"));

   if(!cv.sv_nobossdrop) {
      SpawnBossReward();
   }

   ml.soulsfreed++;

   set_next_score_threshold();

   boss->phase++;
   boss = nil;

   set_msk(ml.flag, _mflg_boss, _mphantom_none);
}

script_str ext("ACS") addr(OBJ "SpawnBoss")
void Z_SpawnBoss(void) {
   if(!boss) return;

   ServCallV(sm_SpawnBoss, fast_strdup2(OBJ "Boss_", boss->name), boss->phase);

   Dbg_Log(log_boss,
           _l("Boss "), _p((cstr)boss->name), _l(" phase "), _p(boss->phase),
           _l(" spawned"));

   set_msk(ml.flag, _mflg_boss, _mphantom_spawned);
}

script static bool chtf_summon_boss(cheat_params_t const params) {
   i32 diff  = params[0] - '0';
   i32 num   = params[1] - '0';
   i32 phase = params[2] - '0';

   if(diff < 0 || diff >= _diff_any ||
      num < 0 || num > 9 ||
      phase < 0 || phase > 9)
   {
      return false;
   }

   switch(params[0] - '0') {
   case _diff_easy: boss = &bosses_easy[num]; break;
   case _diff_medi: boss = &bosses_medi[num]; break;
   case _diff_hard: boss = &bosses_hard[num]; break;
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
