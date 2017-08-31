// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_list.h"
#include "lith_hudid.h"
#include "lith_world.h"

// Extern Objects ------------------------------------------------------------|

[[__no_init]] player_t players[MAX_PLAYERS];

// Static Objects ------------------------------------------------------------|

static struct {__str on, off;} Lith_GUISounds[GUI_MAX] = {
   {},
   {"player/cbi/open", "player/cbi/close"},
};

// Static Functions ----------------------------------------------------------|

[[__call("ScriptS")]] static void Lith_PlayerRunScripts(player_t *p);
static void Lith_PlayerScore(player_t *p);
static void Lith_PlayerStats(player_t *p);
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

   Lith_BossWarning(p);

   while(p->active)
   {
      if(p->reinit)
         goto reinit;

      Lith_PlayerUpdateData(p);

      // This can be changed any time, so save it here.
      player_delta_t olddelta = p->cur;

      // Run logic and rendering
      Lith_PlayerRunScripts(p);

      // Update view
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) - p->addroll);

      // Tic passes
      ACS_Delay(1);

      // Update previous-tic values
      p->old = olddelta;

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

      for(;;)
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
   p->hudstrlist.free(free);
   p->loginfo.full.free(free);
   p->loginfo.maps.free(free);

   upgrademap_t_dtor(&p->upgrademap);

   memset(p, 0, sizeof(*p));
}

// Extern Functions ----------------------------------------------------------|

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
   return p->upgrademap.find(name);
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
   default: return null;
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
      if(world.pauseinmenus) {
         Lith_ScriptCall("Lith_PauseManager", "SetPaused", false);
         Lith_ForPlayer() p->frozen--;
      } else {
         p->frozen--;
      }

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
      if(world.pauseinmenus) {
         Lith_ScriptCall("Lith_PauseManager", "SetPaused", true);
         Lith_ForPlayer() p->frozen++;
      } else {
         p->frozen++;
      }

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
// Lith_GetModScore
//
score_t Lith_GetModScore(player_t *p, score_t score, bool nomul)
{
   // Multiply score by the player's multiplier, and the global multiplier
   if(!nomul)
      score *= p->scoremul;

   return score * world.scoremul;
}

//
// Lith_GiveScore
//
void Lith_GiveScore(player_t *p, score_t score, bool nomul)
{
   // Could cause division by zero
   if(score == 0)
      return;

   score = p->getModScore(score, nomul);

   // Get a multiplier for the score accumulator and sound volume
   double mul = minmax(minmax(score, 0, 20000) / 20000.0f, 0.1f, 1.0f);
   double vol = 0.7 * mul;

   // Play a sound when we pick up score
   if(!IsSmallNumber(vol) && p->getCVarI("lith_player_scoresound"))
      ACS_PlaySound(p->cameratid, "player/score", CHAN_ITEM, vol, false, ATTN_STATIC);

   //
   if(p->getUpgr(UPGR_CyberLegs)->active && ACS_Random(0, 10000) == 0)
   {
      p->brouzouf += score;
      p->log("> You gained brouzouf.");
   }

   if(p->getUpgr(UPGR_TorgueMode)->active && ACS_Random(0, 10) == 0)
   {
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
// Lith_TakeScore
//
void Lith_TakeScore(player_t *p, score_t score)
{
   if(p->score - score >= 0)
   {
      p->scoreused += score;
      p->score     -= score;
   }
   else
   {
      p->scoreused += p->score;
      p->score      = 0;
   }

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
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
                         extern void Lith_PlayerItemFx(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerDamageBob(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerView(player_t *p);
                         extern void Lith_PlayerStyle(player_t *p);
   [[__call("ScriptS")]] extern void Lith_PlayerHUD(player_t *p);
                         extern void Lith_PlayerFootstep(player_t *p);

   // Pre-logic: Update data from the engine.
   Lith_PlayerUpdateWeapon(p); // Update weapon info
   Lith_PlayerScore(p);        // Update score

   if(ACS_Timer() > 4)
      Lith_PlayerStats(p); // Update statistics

   if(!p->dead)
   {
      // Logic: Update our data.
      switch(p->activegui)
      {
      case GUI_CBI: Lith_PlayerUpdateCBIGUI(p); break;
      }

      Lith_PlayerUpdateUpgrades(p); // Update upgrades
      Lith_PlayerUpdateWeapons(p);  // Update weapons
      Lith_PlayerUpdateLog(p);      // Update log data

      // Post-logic: Update the engine's data.
      Lith_PlayerDeltaStats(p); // Update delta'd info

      if(world.pauseinmenus)
         Lith_ScriptCall("Lith_PauseManager", "PauseTick", ACS_PlayerNumber());
   }

   // Rendering
   Lith_PlayerFootstep(p);       // Footstep effects
   Lith_PlayerItemFx(p);         // Update item effects
   Lith_PlayerDamageBob(p);      // Update damage bobbing
   Lith_PlayerView(p);           // Update additive view
   Lith_PlayerRenderUpgrades(p); // Render Upgrades
   Lith_PlayerHUD(p);            // Draw HUD
   Lith_PlayerStyle(p);          // Change player render style
}

//
// Lith_PlayerScore
//
static void Lith_PlayerScore(player_t *p)
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
// Lith_PlayerStats
//
static void Lith_PlayerStats(player_t *p)
{
   if(p->health < p->old.health)
      p->healthused += p->old.health - p->health;
   else if(p->health > p->old.health && ACS_Timer() != 1)
      p->healthsum += p->health - p->old.health;

   if(p->armor < p->old.armor)
      p->armorused += p->old.armor - p->armor;
   else if(p->armor > p->old.armor && ACS_Timer() != 1)
      p->armorsum += p->armor - p->old.armor;

   if(p->x != p->old.x) p->unitstravelled += abs(p->x - p->old.x);
   if(p->y != p->old.y) p->unitstravelled += abs(p->y - p->old.y);
   if(p->z != p->old.z) p->unitstravelled += abs(p->z - p->old.z);
}

//
// Lith_PlayerDeltaStats
//
void Lith_PlayerDeltaStats(player_t *p)
{
   if(p->frozen    != p->old.frozen)    ACS_SetPlayerProperty(0, p->frozen > 0, PROP_TOTALLYFROZEN);
   if(p->speedmul  != p->old.speedmul)  ACS_SetActorPropertyFixed(0, APROP_Speed, 0.7 + p->speedmul);
   if(p->jumpboost != p->old.jumpboost) ACS_SetActorPropertyFixed(0, APROP_JumpZ, p->jumpheight * (1 + p->jumpboost));
}

// EOF

