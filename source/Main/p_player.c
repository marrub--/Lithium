// Copyright © 2016-2018 Alison Sanderson, all rights reserved.
#include "common.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"

#include <limits.h>

// Extern Objects ------------------------------------------------------------|

noinit struct player players[MAX_PLAYERS];

// Static Objects ------------------------------------------------------------|

static struct {str on, off;} guisnd[gui_max - 1] = {
   {s"player/cbi/open", s"player/cbi/close"},
};

// Static Functions ----------------------------------------------------------|

static void Lith_PlayerRunScripts(struct player *p);
#if LITHIUM
script static void Lith_BossWarning(struct player *p);
#endif

// Scripts -------------------------------------------------------------------|

script type("enter")
static void Lith_PlayerEntry(void)
{
   if(ACS_GameType() == GAME_TITLE_MAP) return;

   struct player *p = LocalPlayer;

reinit:
   while(!mapinit) ACS_Delay(1);

   p->reset();
   Lith_PlayerLogEntry(p);
   Lith_PlayerEnterUpgrades(p);
   #if LITHIUM
   p->loadData();

   if(p->num == 0)
   {
      world.fun = p->fun;
      ServCallI(sm_Fun, world.fun);
   }

   Lith_BossWarning(p);
   #endif

   while(p->active)
   {
      if(p->reinit)
         goto reinit;

      Lith_PlayerUpdateData(p);

      // Check for resurrect.
      if(p->health > 0 && p->dead)
         p->reinit = true;

      // These can be changed any time, so save them here.
      struct player_delta olddelta = p->cur;
      i32 oldhealth = p->health;
      i32 oldmana   = p->mana;

      // Run logic and rendering
      Lith_PlayerRunScripts(p);

      // Update view (extra precision is required here to ensure accuracy)
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - (float)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - (float)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) - (float)p->addroll);

      // Tic passes
      ACS_Delay(1);

      #if LITHIUM
      if(p->dlgnum)
      {
         script extern void Lith_DialogueVM(struct player *p, i32 dlgnum);

         Lith_DialogueVM(p, p->dlgnum);
         p->dlgnum = 0;
      }
      #endif

      // Update previous-tic values
      p->old       = olddelta;
      p->oldhealth = oldhealth;
      p->oldmana   = oldmana;

      // Reset view for next tic
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + (float)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + (float)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) + (float)p->addroll);

      // If the map changes this we need to make sure it's still correct.
      p->validateTID();

      p->ticks++;
   }
}

script type("death")
static void Lith_PlayerDeath(void)
{
   struct player *p = LocalPlayer;

   p->dead = true;

   Lith_PlayerDeinitUpgrades(p);

   #if LITHIUM
   // unfortunately, we can't keep anything even when we want to
   Lith_PlayerDeallocInventory(p);
   #endif

   if(world.singleplayer || ACS_GetCVar(sc_sv_cooploseinventory))
   {
      Lith_PlayerDeallocUpgrades(p);
      p->bip.deallocate();
      p->score = p->scoreaccum = p->scoreaccumtime = 0;
   }

   #if LITHIUM
   if(world.singleplayer)
   {
      if(ACS_GetCVar(sc_sv_revenge))
      {
         ACS_LocalAmbientSound(ss_player_death1, 127);
         ACS_Delay(35);
         InvGive(so_PlayerDeath, 1);
         ACS_Delay(25);
         InvGive(so_PlayerDeathNuke, 1);
         ACS_Delay(25);
      }

      while(p->dead)
      {
         ACS_Delay(35 * 5);
         Log("%S", Language(LANG "DEATHMSG_%.2i", ACS_Random(1, 20)));
      }
   }
   #endif
}

script type("respawn")
static void Lith_PlayerRespawn(void)
{
   LocalPlayer->reinit = true;
}

script type("return")
static void Lith_PlayerReturn(void)
{
   LocalPlayer->reinit = true;
}

script type("disconnect")
static void Lith_PlayerDisconnect(void)
{
   struct player *p = LocalPlayer;

   p->bip.deallocate();

   p->hudstrlist.free(true);

   upgrademap_t_dtor(&p->upgrademap);

   memset(p, 0, sizeof *p);
}

// Extern Functions ----------------------------------------------------------|

#define upgrademap_t_GetKey(o) ((o)->info->key)
#define upgrademap_t_GetNext(o) (&(o)->next)
#define upgrademap_t_GetPrev(o) (&(o)->prev)
#define upgrademap_t_HashKey(k) (k)
#define upgrademap_t_HashObj(o) ((o)->info->key)
#define upgrademap_t_KeyCmp(l, r) ((l) - (r))
GDCC_HashMap_Defn(upgrademap_t, i32, upgrade_t)

script ext("ACS")
void Lith_DrawPlayerIcon(int num, int x, int y)
{
   withplayer(&players[num])
   {
      k32 a = absk((x - 160) / 90.0);
           if(a < 0.2) a = 0.2;
      else if(a > 1.0) a = 1.0;

      PrintTextFmt("%S <%i>\n", p->name, p->num);
      __nprintf(p->health <= 0 ? "Dead\n" : "%iHP\n", p->health);
      if(p->pclass & pcl_magicuser) __nprintf("%iMP\n", p->mana);
      PrintTextA(s_cbifont, CR_WHITE, x-9,1, y-2,1, a);
   }
}

stkcall
upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, i32 name)
{
   upgrade_t *upgr = p->upgrademap.find(name);
   if(!upgr) Log("null pointer trying to find upgrade %i", name);
   return upgr;
}

stkcall
bool Lith_PlayerGetUpgradeActive(struct player *p, i32 name)
{
   ifauto(upgrade_t *, upgr, p->upgrademap.find(name)) return upgr->active;
   else                                                return false;
}

struct player (*Lith_GetPlayersExtern(void))[MAX_PLAYERS]
{
   return &players;
}

stkcall
char const *Lith_PlayerDiscriminator(i32 pclass)
{
   switch(pclass) {
   case pcl_marine:    return "Stan";
   case pcl_cybermage: return "Jem";
   case pcl_informant: return "Fulk";
   case pcl_wanderer:  return "Luke";
   case pcl_assassin:  return "Omi";
   case pcl_darklord:  return "Ari";
   case pcl_thoth:     return "Kiri";
   }
   return "Mod";
}

struct player *Lith_GetPlayer(i32 tid, i32 ptr)
{
   i32 pnum = Lith_GetPlayerNumber(tid, ptr);
   if(pnum >= 0) return &players[pnum];
   else          return nil;
}

stkcall
void Lith_PlayerCloseGUI(struct player *p)
{
   if(p->activegui != gui_none)
   {
      if(world.pauseinmenus)
      {
         ServCallI(sm_SetPaused, false);
         Lith_ForPlayer() p->frozen--;
      }
      else
         p->frozen--;

      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      p->activegui = gui_none;
   }
}

stkcall
void Lith_PlayerUseGUI(struct player *p, i32 type)
{
   if(p->dead) return;
   if(p->activegui == gui_none)
   {
      if(world.pauseinmenus)
      {
         ServCallI(sm_SetPaused, true);
         Lith_ForPlayer() p->frozen++;
      }
      else
         p->frozen++;

      if(ACS_Random(0, 10000) == 777)
         ACS_LocalAmbientSound(ss_player_cbi_win95, 127);
      else
         ACS_LocalAmbientSound(guisnd[type - 1].on, 127);
      p->activegui = type;
   }
   else if(p->activegui == type)
      p->closeGUI();
   else
   {
      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      ACS_LocalAmbientSound(guisnd[type - 1].on, 127);
      p->activegui = type;
   }
}

i96 Lith_GiveScore(struct player *p, i96 score, bool nomul)
{
   // Could cause division by zero
   if(score == 0)
      return 0;

   if(!nomul) {
      score *= p->scoremul;
      score *= 1 + (k64)ACS_RandomFixed(0, p->attr.attrs[at_luk] / 77.7);
      score *= world.scoremul;
   }

   // Get a multiplier for the score accumulator and sound volume
   k64 mul = minmax(score, 0, 15000) / 15000.0lk;
           mul = minmax(mul, 0.1lk, 1.0lk);
   k64 vol = 0.7lk * mul;

   // Play a sound when we pick up score
   if(vol > 0.001lk && p->getCVarI(sc_player_scoresound))
      ACS_PlaySound(p->cameratid, ss_player_score, CHAN_ITEM, vol, false, ATTN_STATIC);

   // hue
   if(p->getUpgrActive(UPGR_CyberLegs) && ACS_Random(0, 10000) == 0) {
      p->brouzouf += score;
      p->logB(1, "You gained brouzouf.");
   }

   if(p->getUpgrActive(UPGR_TorgueMode) && ACS_Random(0, 10) == 0) {
      p->spuriousexplosions++;
      ACS_SpawnForced(so_EXPLOOOSION, p->x, p->y, p->z);
   }

   // Add score and set score accumulator
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 20 * (mul * 2.0lk);

   // Log score
   if(p->getCVarI(sc_player_scorelog))
      p->logH(1, "+\Cj%lli\Cnscr", score);

   return score;
}

stkcall
void Lith_TakeScore(struct player *p, i96 score)
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

// Static Functions ----------------------------------------------------------|

#if LITHIUM
script
static void Lith_BossWarning(struct player *p)
{
   ACS_Delay(35 * 5);

   if(world.bossspawned)
      p->logB(1, LanguageC(LANG "LOG_BossWarn%s", p->discrim));
}
#endif

// Run main loop scripts.
static void Lith_PlayerRunScripts(struct player *p)
{
   script extern void Lith_PlayerPreWeapons(struct player *p);
   script static void Lith_PlayerPreScore(struct player *p);
   script static void Lith_PlayerPreStats(struct player *p);

   script extern void Lith_PlayerUpdateCBIGUI(struct player *p);
   script extern void Lith_PlayerUpdateInventory(struct player *p);
   script static void Lith_PlayerUpdateAttributes(struct player *p);
   script extern void Lith_PlayerUpdateUpgrades(struct player *p);
   script extern void Lith_PlayerUpdateWeapons(struct player *p);
   script extern void Lith_PlayerUpdateLog(struct player *p);

   script  extern void Lith_PlayerFootstep(struct player *p);
   stkcall extern void Lith_PlayerItemFx(struct player *p);
   script  extern void Lith_PlayerDamageBob(struct player *p);
   script  extern void Lith_PlayerView(struct player *p);
   script  extern void Lith_PlayerRenderUpgrades(struct player *p);
   script  extern void Lith_PlayerHUD(struct player *p);
   script  extern void Lith_PlayerStyle(struct player *p);
   script  extern void Lith_PlayerLevelup(struct player *p);
   script  extern void Lith_PlayerDebugStats(struct player *p);

   // Pre-logic: Update data from the engine.
   #if LITHIUM
   Lith_PlayerPreWeapons(p); // Update weapon info
   #endif
   Lith_PlayerPreScore(p);   // Update score

   if(ACS_Timer() > 4)
      Lith_PlayerPreStats(p); // Update statistics

   if(!p->dead)
      Lith_PlayerUpdateUpgrades(p);

   Lith_PlayerRenderUpgrades(p);

   if(!p->dead)
   {
      #if LITHIUM
      // Logic: Update our data.
      Lith_PlayerUpdateInventory(p);
      #endif

      switch(p->activegui)
      case gui_cbi: Lith_PlayerUpdateCBIGUI(p);

      Lith_PlayerUpdateAttributes(p);
      #if LITHIUM
      Lith_PlayerUpdateWeapons(p);
      #endif
      Lith_PlayerUpdateLog(p);

      // Post-logic: Update the engine's data.
      Lith_PlayerUpdateStats(p); // Update engine info

      if(world.pauseinmenus) ServCallI(sm_PauseTick, p->num);
   }

   // Rendering
   Lith_PlayerFootstep(p);
   #if LITHIUM
   Lith_PlayerItemFx(p);
   #endif
   Lith_PlayerDamageBob(p);
   Lith_PlayerView(p);
   #if LITHIUM
   Lith_PlayerHUD(p);
   Lith_PlayerStyle(p);
   #endif
   Lith_PlayerLevelup(p);
   Lith_PlayerDebugStats(p);
}

stkcall
static void AttrRGE(struct player *p)
{
   i32 rge = p->attr.attrs[at_spc];

   if(p->health < p->oldhealth)
      p->rage += rge * (p->oldhealth - p->health) / 1000.0;

   p->rage = lerpk(p->rage, 0, 0.02);
}

stkcall
static void AttrCON(struct player *p)
{
   i32 rge = p->attr.attrs[at_spc];

   if(p->mana > p->oldmana)
      p->rage += rge * (p->mana - p->oldmana) / 1100.0;

   p->rage = lerpk(p->rage, 0, 0.03);
}

script
static void Lith_PlayerUpdateAttributes(struct player *p)
{
   if(Lith_IsPaused) return;

   k32  acc = p->attr.attrs[at_acc] / 150.0;
   k32  def = p->attr.attrs[at_def] / 170.0;
   i32 strn = p->attr.attrs[at_str];
   i32  stm = p->attr.attrs[at_stm];
   i32 stmt = 75 - stm;

   switch(p->pclass) {
   case pcl_marine:    AttrRGE(p); break;
   case pcl_cybermage: AttrCON(p); break;
   }

   p->maxhealth = p->spawnhealth + strn;
   SetPropK(0, APROP_DamageMultiplier, 1.0 + acc + p->rage);
   SetMembI(0, sm_DmgFac, minmax(100 * def, 0, 100));
   SetPropI(0, APROP_SpawnHealth, p->maxhealth);

   if(p->health < stm+10 && (stmt < 2 || p->ticks % stmt == 0))
      p->health = p->health + 1;
}

script
static void Lith_PlayerPreScore(struct player *p)
{
   if(!p->scoreaccumtime || p->score < p->old.score)
   {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }

        if(p->scoreaccumtime > 0) p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0) p->scoreaccumtime++;
}

script
static void Lith_PlayerPreStats(struct player *p)
{
        if(p->health < p->oldhealth                    ) p->healthused += p->oldhealth - p->health;
   else if(p->health > p->oldhealth && ACS_Timer() != 1) p->healthsum  += p->health    - p->oldhealth;

   if(p->x != p->old.x) p->unitstravelled += abs(p->x - p->old.x);
   if(p->y != p->old.y) p->unitstravelled += abs(p->y - p->old.y);
   if(p->z != p->old.z) p->unitstravelled += abs(p->z - p->old.z);
}

// EOF
