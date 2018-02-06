// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_world.h"

#include <limits.h>

// Extern Objects ------------------------------------------------------------|

[[__no_init]] player_t players[MAX_PLAYERS];

// Static Objects ------------------------------------------------------------|

static struct {__str on, off;} Lith_GUISounds[GUI_MAX] = {
   {},
   {"player/cbi/open", "player/cbi/close"},
};

// Callbacks -----------------------------------------------------------------|

CallbackDefine(player_cb_t, PlayerUpdate)
CallbackDefine(player_cb_t, PlayerRender)

// Static Functions ----------------------------------------------------------|

[[__call("ScriptS")]] static void Lith_PlayerRunScripts(player_t *p);
[[__call("ScriptS")]] static void Lith_BossWarning(player_t *p);

// Scripts -------------------------------------------------------------------|

//
// Lith_PlayerEntry
//
[[__call("ScriptS"), __script("Enter")]]
static void Lith_PlayerEntry(void)
{
   if(ACS_GameType() == GAME_TITLE_MAP)
      return;

   player_t *p = LocalPlayer;

reinit:
   while(!mapinit) ACS_Delay(1);

   p->reset();
   Lith_PlayerLogEntry(p);
   Lith_PlayerEnterUpgrades(p);
   p->loadData();

   if(p->num == 0)
   {
      world.fun = p->fun;
      HERMES("Fun", world.fun);
   }

   Lith_BossWarning(p);

   while(p->active)
   {
      if(p->reinit)
         goto reinit;

      Lith_PlayerUpdateData(p);

      // Check for resurrect.
      if(p->health > 0 && p->dead)
         p->reinit = true;

      // These can be changed any time, so save them here.
      player_delta_t olddelta = p->cur;
      int oldhealth = p->health;

      // Run logic and rendering
      Lith_PlayerRunScripts(p);

      // Update view
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) - p->addroll);

      // Tic passes
      ACS_Delay(1);

      if(p->dlgnum)
      {
         [[__call("ScriptS")]] extern void Lith_DialogueVM(player_t *p, int dlgnum);

         Lith_DialogueVM(p, p->dlgnum);
         p->dlgnum = 0;
      }

      // Update previous-tic values
      p->old       = olddelta;
      p->oldhealth = oldhealth;

      // Reset view for next tic
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) + p->addroll);

      // If the map changes this we need to make sure it's still correct.
      p->validateTID();

      p->ticks++;
   }
}

//
// Lith_PlayerDeath
//
[[__call("ScriptS"), __script("Death")]]
static void Lith_PlayerDeath(void)
{
   player_t *p = LocalPlayer;

   p->dead = true;

   Lith_PlayerDeinitUpgrades(p);

   if(world.singleplayer || ACS_GetCVar("sv_cooploseinventory"))
   {
      Lith_PlayerDeallocUpgrades(p);
      p->bip.deallocate();
      p->score = p->scoreaccum = p->scoreaccumtime = 0;
   }

   if(world.singleplayer)
   {
      if(ACS_GetCVar("lith_sv_revenge"))
      {
         ACS_LocalAmbientSound("player/death1", 127);
         ACS_Delay(35);
         ACS_GiveInventory("Lith_PlayerDeath", 1);
         ACS_Delay(25);
         ACS_GiveInventory("Lith_PlayerDeathNuke", 1);
         ACS_Delay(25);
      }

      while(p->dead)
      {
         ACS_Delay(35 * 5);
         Log("%S", Language("LITH_DEATHMSG_%.2i", ACS_Random(1, 20)));
      }
   }
}

//
// Lith_PlayerRespawn
//
[[__call("ScriptS"), __script("Respawn")]]
static void Lith_PlayerRespawn(void)
{
   LocalPlayer->reinit = true;
}

//
// Lith_PlayerReturn
//
[[__call("ScriptS"), __script("Return")]]
static void Lith_PlayerReturn(void)
{
   LocalPlayer->reinit = true;
}

//
// Lith_PlayerDisconnect
//
[[__call("ScriptS"), __script("Disconnect")]]
static void Lith_PlayerDisconnect(void)
{
   player_t *p = LocalPlayer;

   p->bip.deallocate();

   p->loginfo.hud.free();
   p->hudstrlist.free(true);
   p->loginfo.full.free(true);
   p->loginfo.maps.free(true);

   upgrademap_t_dtor(&p->upgrademap);

   memset(p, 0, sizeof(*p));
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_Obituary
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_Obituary(void)
{
   static __str const pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"he",   "him",  "his",   "his",    "he's"},
      {"she",  "her",  "her",   "hers",   "she's"},
      {"it",   "it",   "its",   "its'",   "it's"},
   };

   player_t *p = LocalPlayer;

   __str obit = HERMES_S("GetObituary");
   if(obit == "") return;

   if(obit == "(falling)")
      obit = Language("LITH_OB_Falling_%i", ACS_Random(1, 5));
   else if(obit == "(crush)")
      obit = Language("LITH_OB_Crush_%i", ACS_Random(1, 5));
   else if(obit == "(exit)")
      obit = Language("LITH_OB_Exit_%i", ACS_Random(1, 5));
   else if(obit == "(drowning)")
      obit = Language("LITH_OB_Drowning_%i", ACS_Random(1, 5));
   else if(obit == "(slime)")
      obit = Language("LITH_OB_Slime_%i", ACS_Random(1, 5));
   else if(obit == "(fire)")
      obit = Language("LITH_OB_Fire_%i", ACS_Random(1, 5));
   else if(obit == "(suicide)")
      obit = Language("LITH_OB_Suicide_%i", ACS_Random(1, 5));
   else if(obit == "(default)")
      obit = Language("LITH_OB_Default_%i", ACS_Random(1, 5));

   ACS_BeginPrint();

   for(char __str_ars const *s = obit; *s;)
   {
      if(s[0] == '%') switch(s[1])
      {
      case 'o': s += 2; ACS_PrintName(p->num);                   continue;
      case 'g': s += 2; ACS_PrintString(pronoun[p->pronoun][0]); continue;
      case 'h': s += 2; ACS_PrintString(pronoun[p->pronoun][1]); continue;
      case 'p': s += 2; ACS_PrintString(pronoun[p->pronoun][2]); continue;
      case 's': s += 2; ACS_PrintString(pronoun[p->pronoun][3]); continue;
      case 'r': s += 2; ACS_PrintString(pronoun[p->pronoun][4]); continue;
      }
      ACS_PrintChar(*(s++));
   }

   obit = ACS_EndStrParam();

   LogDebug(log_dev, "%S", obit);
   Lith_ForPlayer() p->log("> %S", obit);
}

#define upgrademap_t_GetKey(o) ((o)->info->key)
#define upgrademap_t_GetNext(o) (&(o)->next)
#define upgrademap_t_GetPrev(o) (&(o)->prev)
#define upgrademap_t_HashKey(k) (k)
#define upgrademap_t_HashObj(o) ((o)->info->key)
#define upgrademap_t_KeyCmp(l, r) ((l) - (r))
GDCC_HashMap_Defn(upgrademap_t, int, upgrade_t)

//
// Lith_PlayerGetNamedUpgrade
//
upgrade_t *Lith_PlayerGetNamedUpgrade(player_t *p, int name)
{
   upgrade_t *upgr = p->upgrademap.find(name);
   if(!upgr) Log("null pointer trying to find upgrade %i", name);
   return upgr;
}

//
// Lith_PlayerGetUpgradeActive
//
bool Lith_PlayerGetUpgradeActive(player_t *p, int name)
{
   ifauto(upgrade_t *, upgr, p->upgrademap.find(name)) return upgr->active;
   else                                                return false;
}

//
// Lith_GetPlayersExtern
//
player_t (*Lith_GetPlayersExtern(void))[MAX_PLAYERS]
{
   return &players;
}

//
// Lith_PlayerDiscriminator
//
__str Lith_PlayerDiscriminator(int pclass)
{
   switch(pclass) {
   case pcl_marine:    return "Stan";
   case pcl_cybermage: return "Jem";
   case pcl_informant: return "Fulk";
   case pcl_wanderer:  return "Luke";
   case pcl_assassin:  return "Omi";
   case pcl_darklord:  return "Ari";
   case pcl_thoth:     return "Kiri";
   default:            return "Mod";
   }
}

//
// Lith_StepSpeed
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_StepSpeed()
{
   player_t *p = LocalPlayer;

	fixed vel = ACS_VectorLength(absk(p->velx), absk(p->vely));
   fixed num = 1k - (vel / 24k);
	fixed mul = minmax(num, 0.35k, 1k);

	return 6 * (mul + 0.6k);
}

//
// Lith_GetPlayer
//
player_t *Lith_GetPlayer(int tid, int ptr)
{
   int pnum = Lith_GetPlayerNumber(tid, ptr);
   if(pnum >= 0) return &players[pnum];
   else          return null;
}

//
// Lith_PlayerCloseGUI
//
void Lith_PlayerCloseGUI(player_t *p)
{
   if(p->activegui != GUI_NONE)
   {
      if(world.pauseinmenus)
      {
         HERMES("SetPaused", false);
         Lith_ForPlayer() p->frozen--;
      }
      else
         p->frozen--;

      ACS_LocalAmbientSound(Lith_GUISounds[p->activegui].off, 127);
      p->activegui = GUI_NONE;
   }
}

//
// Lith_PlayerUseGUI
//
void Lith_PlayerUseGUI(player_t *p, guiname_t type)
{
   if(p->dead) return;
   if(p->activegui == GUI_NONE)
   {
      if(world.pauseinmenus)
      {
         HERMES("SetPaused", true);
         Lith_ForPlayer() p->frozen++;
      }
      else
         p->frozen++;

      ACS_LocalAmbientSound(Lith_GUISounds[type].on, 127);
      p->activegui = type;
   }
   else if(p->activegui == type)
      p->closeGUI();
   else
   {
      ACS_LocalAmbientSound(Lith_GUISounds[p->activegui].off, 127);
      ACS_LocalAmbientSound(Lith_GUISounds[type].on, 127);
      p->activegui = type;
   }
}

//
// Lith_GiveScore
//
score_t Lith_GiveScore(player_t *p, score_t score, bool nomul)
{
   #pragma GDCC FIXED_LITERAL OFF
   // Could cause division by zero
   if(score == 0)
      return 0;

   if(!nomul) {
      score *= p->scoremul;
      score *= 1 + (double)RandomFloat(0, p->attr.attrs[at_luk] / 77.7);
      score *= world.scoremul;
   }

   // Get a multiplier for the score accumulator and sound volume
   double mul = minmax(score, 0, 15000) / 15000.0;
          mul = minmax(mul, 0.1, 1.0);
   double vol = 0.7 * mul;

   // Play a sound when we pick up score
   if(!IsSmallNumber(vol) && p->getCVarI("lith_player_scoresound"))
      ACS_PlaySound(p->cameratid, "player/score", CHAN_ITEM, vol, false, ATTN_STATIC);

   //
   if(p->getUpgrActive(UPGR_CyberLegs) && ACS_Random(0, 10000) == 0) {
      p->brouzouf += score;
      p->log("> You gained brouzouf.");
   }

   if(p->getUpgrActive(UPGR_TorgueMode) && ACS_Random(0, 10) == 0) {
      p->spuriousexplosions++;
      ACS_SpawnForced("Lith_EXPLOOOSION", p->x, p->y, p->z);
   }

   // Add score and set score accumulator
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 20 * (mul * 2.0);

   // Log score
   if(p->getCVarI("lith_player_scorelog"))
      p->logH("> +\Cj%lli\Cnscr", score);

   return score;
}

//
// Lith_TakeScore
//
void Lith_TakeScore(player_t *p, score_t score)
{
   if(p->score - score >= 0) {
      p->scoreused += score;
      p->score     -= score;
   } else {
      p->scoreused += p->score;
      p->score      = 0;
   }

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
}

//
// Lith_GiveMeAllOfTheScore
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveMeAllOfTheScore(void)
{
   withplayer(LocalPlayer) p->giveScore(0x7FFFFFFFFFFFFFFFFFFFFFFFLL, true);
}

//
// Lith_GiveHealthBonus
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveHealthBonus(int pnum, int amount)
{
   withplayer(&players[pnum])
   {
      amount += p->health;
      if(amount > p->maxhealth + 100) amount = p->maxhealth + 100;
      p->health = amount;
   }
}

//
// Lith_GiveHealth
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveHealth(int pnum, int amount)
{
   withplayer(&players[pnum])
   {
      amount += p->health;
      amount *= 1 + p->attr.attrs[at_vit] / 80.0;
      if(amount > p->maxhealth) amount = p->maxhealth;
      p->health = amount;
   }
}

//
// Lith_CheckHealth
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckHealth(int pnum)
{
   withplayer(&players[pnum]) return p->health < p->maxhealth;
   return 0;
}

//
// Lith_Discount
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_Discount(int pnum)
{
   withplayer(&players[pnum])
      p->discount = 0.9;
}

// Static Functions ----------------------------------------------------------|

//
// Lith_BossWarning
//
[[__call("ScriptS")]]
static void Lith_BossWarning(player_t *p)
{
   ACS_Delay(35 * 5);

   if(world.bossspawned)
      p->log("%S", Language("LITH_TXT_LOG_BossWarn%S", p->discrim));
}

//
// Lith_PlayerRunScripts
//
// Run main loop scripts.
//
[[__call("ScriptS")]]
static void Lith_PlayerRunScripts(player_t *p)
{
   [[__call("ScriptS")]] extern void Lith_PlayerPreWeapons(player_t *p);
   [[__call("ScriptS")]] static void Lith_PlayerPreScore(player_t *p);
   [[__call("ScriptS")]] static void Lith_PlayerPreStats(player_t *p);

   [[__call("ScriptS")]] extern void Lith_PlayerUpdateCBIGUI(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerUpdateInventory(player_t *p);
   [[__call("ScriptS")]] static void Lith_PlayerUpdateAttributes(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerUpdateUpgrades(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerUpdateWeapons(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerUpdateLog(player_t *p);

   [[__call("ScriptS")]] extern void Lith_PlayerFootstep(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerItemFx(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerDamageBob(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerView(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerRenderUpgrades(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerHUD(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerStyle(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerLevelup(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerDebugStats(player_t *p);

   // Pre-logic: Update data from the engine.
   Lith_PlayerPreWeapons(p); // Update weapon info
   Lith_PlayerPreScore(p);   // Update score

   if(ACS_Timer() > 4)
      Lith_PlayerPreStats(p); // Update statistics

   if(!p->dead)
      Lith_PlayerUpdateUpgrades(p);

   Lith_PlayerRenderUpgrades(p);

   if(!p->dead)
   {
      // Logic: Update our data.
      Lith_PlayerUpdateInventory(p);

      switch(p->activegui)
      case GUI_CBI: Lith_PlayerUpdateCBIGUI(p);

      CallbackRun(player_cb_t, PlayerUpdate, p);

      Lith_PlayerUpdateAttributes(p);
      Lith_PlayerUpdateWeapons(p);
      Lith_PlayerUpdateLog(p);

      // Post-logic: Update the engine's data.
      Lith_PlayerUpdateStats(p); // Update engine info

      if(world.pauseinmenus) HERMES("PauseTick", p->num);
   }

   // Rendering
   CallbackRun(player_cb_t, PlayerRender, p);

   Lith_PlayerFootstep(p);
   Lith_PlayerItemFx(p);
   Lith_PlayerDamageBob(p);
   Lith_PlayerView(p);
   Lith_PlayerHUD(p);
   Lith_PlayerStyle(p);
   Lith_PlayerLevelup(p);
   Lith_PlayerDebugStats(p);
}

//
// Lith_PlayerUpdateAttributes
//
[[__call("ScriptS")]]
static void Lith_PlayerUpdateAttributes(player_t *p)
{
   fixed acc = p->attr.attrs[at_acc] / 210.0;
   fixed def = p->attr.attrs[at_def] / 290.0;
   int   str = p->attr.attrs[at_str];
   int   stm = p->attr.attrs[at_stm];
   int  stmt = (ATTR_MAX*1.25 - stm) / (fixed)(ATTR_MAX / 4) * 15;
   int   rge = p->attr.attrs[at_rge];

   if(p->health < p->oldhealth)
      p->rage += rge * (p->oldhealth - p->health) / 1000.0;

   p->maxhealth = p->spawnhealth + str;
   ACS_SetActorPropertyFixed(0, APROP_DamageMultiplier, 1.0 + acc + p->rage);
   ACS_SetActorPropertyFixed(0, APROP_DamageFactor,     p->spawndfactor - def);
   ACS_SetActorProperty     (0, APROP_SpawnHealth, p->maxhealth);

   if(p->health < stm+1 && (!stmt || p->ticks % stmt == 0))
      p->health = p->health + 1;

   p->rage = lerpk(p->rage, 0, 0.02);
}

//
// Lith_PlayerPreScore
//
[[__call("ScriptS")]]
static void Lith_PlayerPreScore(player_t *p)
{
   if(!p->scoreaccumtime || p->score < p->old.score)
   {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }

   if(p->scoreaccumtime > 0)
      p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0)
      p->scoreaccumtime++;
}

//
// Lith_PlayerPreStats
//
[[__call("ScriptS")]]
static void Lith_PlayerPreStats(player_t *p)
{
   if(p->health < p->oldhealth)
      p->healthused += p->oldhealth - p->health;
   else if(p->health > p->oldhealth && ACS_Timer() != 1)
      p->healthsum += p->health - p->oldhealth;

   if(p->x != p->old.x) p->unitstravelled += abs(p->x - p->old.x);
   if(p->y != p->old.y) p->unitstravelled += abs(p->y - p->old.y);
   if(p->z != p->old.z) p->unitstravelled += abs(p->z - p->old.z);
}

// EOF

