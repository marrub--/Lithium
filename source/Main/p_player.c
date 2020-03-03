/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "m_list.h"
#include "p_hudid.h"
#include "w_world.h"
#include "d_vm.h"

#include <limits.h>

/* Extern Objects ---------------------------------------------------------- */

noinit struct player players[MAX_PLAYERS];

/* Static Objects ---------------------------------------------------------- */

static struct {str on, off;} guisnd[gui_max - 1] = {
   {s"player/cbi/open", s"player/cbi/close"},
};

/* Static Functions -------------------------------------------------------- */

#if LITHIUM
script static void P_BossWarning(struct player *p);
#endif

/* Scripts ----------------------------------------------------------------- */

script type("enter")
static void Sc_PlayerEntry(void)
{
   script  extern void P_Wep_PTickPre(struct player *p);
   stkcall extern void P_Dat_PTickPre(struct player *p);
   stkcall static void P_Scr_PTickPre(struct player *p);
   script  extern void P_CBI_PTick   (struct player *p);
   script  extern void P_Inv_PTick   (struct player *p);
   script  extern void P_Log_PTick   (struct player *p);
   script  extern void P_Upg_PTick   (struct player *p);
   script  extern void P_Wep_PTick   (struct player *p);
   stkcall static void P_Atr_PTick   (struct player *p);
   script  extern void P_Upg_PTickPst(struct player *p);
   stkcall extern void P_Ren_PTickPst(struct player *p);

   if(ACS_GameType() == GAME_TITLE_MAP) return;

   struct player *p = LocalPlayer;

reinit:
   while(!player_init) ACS_Delay(1);

   P_Init(p);
   P_Log_Entry(p);
   P_Upg_Enter(p);
   #if LITHIUM
   P_Data_Load(p);

   if(p->num == 0)
   {
      fun = p->fun;
      ServCallI(sm_Fun, fun);
   }

   P_BossWarning(p);
   #endif

   if(p->teleportedout) P_TeleportInAsync(p);

   while(p->active)
   {
      if(p->reinit)
         goto reinit;

      P_Dat_PTickPre(p);

      /* Check for resurrect. */
      if(p->health > 0 && p->dead)
         p->reinit = true;

      /* These can be changed any time, so save them here. */
      struct player_delta olddelta = p->cur;
      i32 oldhealth = p->health;
      i32 oldmana   = p->mana;

      /* Pre-tick */
      #if LITHIUM
      P_Wep_PTickPre(p); /* Update weapon info */
      #endif
      P_Scr_PTickPre(p); /* Update score */

      if(!p->dead) P_Upg_PTick(p);
      P_Upg_PTickPst(p);

      /* Tick */
      if(!p->dead)
      {
         #if LITHIUM
         P_Inv_PTick(p);
         #endif

         switch(p->activegui)
         case gui_cbi: P_CBI_PTick(p);

         P_Atr_PTick(p);
         #if LITHIUM
         P_Wep_PTick(p);
         #endif
         P_Log_PTick(p);

         P_Dat_PTickPst(p); /* Update engine info */

         if(pauseinmenus) ServCallI(sm_PauseTick, p->num);
      }

      /* Post-tick */
      P_Ren_PTickPst(p);

      /* Update view (extra precision is required here to ensure accuracy) */
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) - (f32)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) - (f32)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) - (f32)p->addroll);

      /* Tic passes */
      ACS_Delay(1);

      #if LITHIUM
      if(p->dlg.num) {
         Dlg_Run(p, p->dlg.num);
         p->dlg.num = 0;
      }
      #endif

      /* Update previous-tic values */
      p->old       = olddelta;
      p->oldhealth = oldhealth;
      p->oldmana   = oldmana;

      /* Reset view for next tic */
      ACS_SetActorPitch(0, ACS_GetActorPitch(0) + (f32)p->addpitch);
      ACS_SetActorAngle(0, ACS_GetActorAngle(0) + (f32)p->addyaw);
      ACS_SetActorRoll (0, ACS_GetActorRoll (0) + (f32)p->addroll);

      /* If the map changes this we need to make sure it's still correct. */
      P_ValidateTID(p);

      p->ticks++;
   }
}

script type("death")
static void Sc_PlayerDeath(void)
{
   struct player *p = LocalPlayer;

   p->dead = true;

   P_Upg_PDeinit(p);

   #if LITHIUM
   /* unfortunately, we can't keep anything even when we want to */
   P_Inv_PQuit(p);
   #endif

   if(singleplayer || ACS_GetCVar(sc_sv_cooploseinventory))
   {
      P_Upg_PQuit(p);
      P_BIP_PQuit(p);
      p->score = p->scoreaccum = p->scoreaccumtime = 0;
   }

   #if LITHIUM
   if(singleplayer)
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
static void Sc_PlayerRespawn(void)
{
   LocalPlayer->reinit = true;
}

script type("return")
static void Sc_PlayerReturn(void)
{
   LocalPlayer->reinit = true;
}

script type("disconnect")
static void Sc_PlayerDisconnect(void)
{
   struct player *p = LocalPlayer;

   P_BIP_PQuit(p);

   ListDtor(&p->hudstrlist, true);

   upgrademap_t_dtor(&p->upgrademap);

   fastmemset(p, 0, sizeof *p);
}

/* Extern Functions -------------------------------------------------------- */

#define upgrademap_t_GetKey(o) ((o)->info->key)
#define upgrademap_t_GetNext(o) (&(o)->next)
#define upgrademap_t_GetPrev(o) (&(o)->prev)
#define upgrademap_t_HashKey(k) (k)
#define upgrademap_t_HashObj(o) ((o)->info->key)
#define upgrademap_t_KeyCmp(l, r) ((l) - (r))
GDCC_HashMap_Defn(upgrademap_t, i32, struct upgrade)

stkcall
struct upgrade *P_Upg_GetNamed(struct player *p, i32 name)
{
   struct upgrade *upgr = p->upgrademap.find(name);
   if(!upgr) Log("null pointer trying to find upgrade %i", name);
   return upgr;
}

stkcall
bool P_Upg_IsActive(struct player *p, i32 name)
{
   ifauto(struct upgrade *, upgr, p->upgrademap.find(name)) return upgr->active;
   else                                                return false;
}

stkcall
cstr P_Discrim(i32 pclass)
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

struct player *P_PtrFind(i32 tid, i32 ptr)
{
   i32 pnum = PtrPlayerNumber(tid, ptr);
   if(pnum >= 0) return &players[pnum];
   else          return nil;
}

stkcall
void P_GUI_Close(struct player *p)
{
   if(p->activegui != gui_none)
   {
      if(pauseinmenus)
      {
         ServCallI(sm_SetPaused, false);
         for_player() p->frozen--;
      }
      else
         p->frozen--;

      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      p->activegui = gui_none;
   }
}

stkcall
void P_GUI_Use(struct player *p, i32 type)
{
   if(p->dead) return;
   if(p->activegui == gui_none)
   {
      if(pauseinmenus)
      {
         ServCallI(sm_SetPaused, true);
         for_player() p->frozen++;
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
      P_GUI_Close(p);
   else
   {
      ACS_LocalAmbientSound(guisnd[p->activegui - 1].off, 127);
      ACS_LocalAmbientSound(guisnd[type - 1].on, 127);
      p->activegui = type;
   }
}

i96 P_Scr_Give(struct player *p, i96 score, bool nomul)
{
   /* Could cause division by zero */
   if(score == 0)
      return 0;

   if(!nomul) {
      score *= p->scoremul;
      score *= 1 + (k64)ACS_RandomFixed(0, p->attr.attrs[at_luk] / 77.7);
      score *= scoremul;
   }

   /* Get a multiplier for the score accumulator and sound volume */
   k64 mul = minmax(score, 0, 15000) / 15000.0lk;
           mul = minmax(mul, 0.1lk, 1.0lk);
   k64 vol = 0.7lk * mul;

   /* Play a sound when we pick up score */
   if(vol > 0.001lk && p->getCVarI(sc_player_scoresound))
      ACS_PlaySound(p->cameratid, ss_player_score, CHAN_ITEM, vol, false, ATTN_STATIC);

   /* hue */
   if(p->getUpgrActive(UPGR_CyberLegs) && ACS_Random(0, 10000) == 0) {
      p->brouzouf += score;
      p->logB(1, "You gained brouzouf.");
   }

   if(p->getUpgrActive(UPGR_TorgueMode) && ACS_Random(0, 10) == 0) {
      p->spuriousexplosions++;
      ACS_SpawnForced(so_EXPLOOOSION, p->x, p->y, p->z);
   }

   /* Add score and set score accumulator */
   p->score          += score;
   p->scoresum       += score;
   p->scoreaccum     += score;
   p->scoreaccumtime += 20 * (mul * 2.0lk);

   /* Log score */
   if(p->getCVarI(sc_player_scorelog))
      p->logH(1, "+\Cj%lli\Cnscr", score);

   return score;
}

stkcall
void P_Scr_Take(struct player *p, i96 score)
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

/* Static Functions -------------------------------------------------------- */

#if LITHIUM
script
static void P_BossWarning(struct player *p)
{
   ACS_Delay(35 * 5);

   if(bossspawned)
      p->logB(1, LanguageC(LANG "LOG_BossWarn%s", p->discrim));
}
#endif

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

stkcall
static void P_Atr_PTick(struct player *p)
{
   if(Paused) return;

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

stkcall
static void P_Scr_PTickPre(struct player *p)
{
   if(!p->scoreaccumtime || p->score < p->old.score)
   {
      p->scoreaccum = 0;
      p->scoreaccumtime = 0;
   }

        if(p->scoreaccumtime > 0) p->scoreaccumtime--;
   else if(p->scoreaccumtime < 0) p->scoreaccumtime++;
}

/* Scripts ----------------------------------------------------------------- */

script ext("ACS") addr(lsc_drawplayericon)
void Sc_DrawPlayerIcon(i32 num, i32 x, i32 y)
{
   with_player(&players[num])
   {
      k32 a = absk((x - 160) / 90.0);
           if(a < 0.2) a = 0.2;
      else if(a > 1.0) a = 1.0;

      PrintTextFmt("%S <%i>\n", p->name, p->num);
      __nprintf(p->health <= 0 ? "Dead\n" : "%iHP\n", p->health);
      if(p->pclass & pcl_magicuser) __nprintf("%iMP\n", p->mana);
      PrintTextA(s_smallfnt, CR_WHITE, x-9,1, y-2,1, a);
   }
}

script_str type("net") ext("ACS") addr("Lith_Glare")
void Sc_Glare(void)
{
   with_player(LocalPlayer)
   {
      ACS_FadeTo(255, 255, 255, 1.0, 0.0);

      ACS_LocalAmbientSound(ss_player_glare, 127);
      ACS_LineAttack(0, p->yaw, p->pitch, 1, so_Dummy, s_None,
         32767.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL);

      ACS_Delay(14);

      ACS_FadeTo(255, 255, 255, 0.0, 0.2);

      ACS_Delay(19);
   }
}

/* EOF */
